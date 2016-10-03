// 
// 
// 
#include "os.h"
#include "communication.h"
unsigned short crc::crc16_ccitt(uint8_t *buf, int len, unsigned short crc) // crc: crc seed to start with. Use 0x0000
{
	register int counter;
	for (counter = 0; counter < len; counter++)
		crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ *(uint8_t*)buf++) & 0x00FF];
	return crc;
}
//********************************** Important about speed of transmitting ****************************
//The Tx on Atmega2560 does not have buffer
//Each framed byte has 10-bits. In 10ms 230bytes are transmitted at 230,020bps
//*****************************************************************************************************
void bluetooth_hal::exec_blocking(){
	delay(10);
	while (main_serial.available() == 0) {
	}
	while (main_serial.available() > 0) {
		main_serial.read();
		delay(50);
	}
}
bool bluetooth_hal::check_for_byte(uint8_t &byte_recieved){
	bool byte_available = false;
	if (main_serial.available() > 0) {
		byte_recieved = main_serial.read();
		byte_available = true;
	}
	return byte_available;
}
void bluetooth_hal::send_byte(uint8_t byte_para){
	main_serial.write(byte_para);
}
void bluetooth_hal::send_arr(uint8_t* buf, size_t size){
	main_serial.write(buf, size);
}
void bluetooth_hal::init_bluetooth(){
	main_serial.print("$$$");
	exec_blocking();
	main_serial.print("SN,QUAD\r"); //Set the name of the bluetooth to 'QUAD'
	exec_blocking();
	main_serial.print("SO,CC\r"); //Set prefix of connect and disconnect event
	exec_blocking();
	main_serial.print("SM,0\r"); //Set the device to slave mode
	exec_blocking();
	main_serial.print("SA,0\r");
	exec_blocking();
	main_serial.print("---\r");
	exec_blocking();
}

uint8_t serial_parser::state_machine(byte input_byte){  //General Message:  |  Padding. (2-bytes)  |.  Msg. Len. (1-byte)  | Msg. type (1-byte)   |.  Payload (Variable)    |. Crc (2-bytes).   |
	uint8_t return_status = 0;
	switch (state)
	{
	case Padd_search: //$$$$$$$$$$$$$$$$ Reading Padding $$$$$$$$$$$$$$$$$$
		if (input_byte == padding)
		{
			byte_counter.padding_counter++;
			data_stream[byte_index++] = input_byte;
		}
		else
		{
			byte_counter.padding_counter = 0;
			byte_index = 0;
		}
		if (byte_counter.padding_counter == padding_len)
		{
			byte_counter.padding_counter = 0;
			state = header_read;
		}
		break;
	case header_read://$$$$$$$$$$$$$$$$$ Reading Header $$$$$$$$$$$$$$$$$$$
		data_stream[byte_index++] = input_byte;
		byte_counter.header_counter++;

		if (byte_counter.header_counter == header_len)
		{
			byte_counter.msg_counter = 0;
			state = msg_read;
		}
		else //Because message length is the first byte
		{
			payload_len = ((uint16_t)input_byte) - header_len - padding_len - crc_len;
		}
		break;
	case msg_read: //$$$$$$$$$$$$$$$$ Reading the message $$$$$$$$$$$$$$$$$$$$
		data_stream[byte_index++] = input_byte;
		byte_counter.msg_counter++;
		if (byte_counter.msg_counter == payload_len)
		{
			byte_counter.msg_counter = 0;
			state = crc_read;
		}
		break;
	case crc_read://$$$$$$$$$$$$$$$$$ Reading the CRC $$$$$$$$$$$$$$$$$$$$$$
		data_stream[byte_index++] = input_byte;
		byte_counter.crc_counter++;
		if (byte_counter.crc_counter == crc_len)
		{
			byte_counter.crc_counter = 0;
			state = done_reading;
		}
		break;
	case done_reading:
		uint16_t crc_res = crc::crc16_ccitt(data_stream, byte_index, 0x0000);
		if (crc_res == 0) return_status = 1;
		state = Padd_search;
		break;
	}
	return return_status;
}
void serial_parser::configure(int para_padding_len, int para_header_len, int para_crc_len, uint8_t para_padding)
{
	padding_len = para_padding_len;
	header_len = para_header_len;
	crc_len = para_crc_len;
	padding = para_padding;
}
bool serial_parser::push_byte(uint8_t data, uint8_t* (frame_data), size_t &payload_len_para)
{
	if (state_machine(data) == 1)
	{
		payload_len_para = payload_len;
		for (int i = 0; i <= payload_len - 1; i++){
			*frame_data = *(data_stream + i);
		}
		return true;
	}
	else
	{
		return false;
	}
}
void serial_parser::get_payload(uint8_t* payload_seq, size_t payload_len){
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!! To be implemented !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

uint8_t* serial_composer::get_tx_frame_buf(){
	return frame_bytes;
}
void serial_composer::configure(int para_padding_len, int para_header_len, int para_crc_len, uint8_t para_padding)
{
	padding_len = para_padding_len;
	header_len = para_header_len;
	crc_len = para_crc_len;
	padding = para_padding;
}
void serial_composer::send_packet(void* data_pointer, size_t data_size, uint8_t msg_type_para){ //$$$$$$ QC PASS - without CRC $$$$$$
	//SOURCE: for understanding serial timing https://www.baldengineer.com/when-do-you-use-the-arduinos-to-use-serial-flush.html
	uint16_t crc_res;
	uint8_t frame_len = (uint8_t)(msg_pad_len + msg_hdr_len + data_size + msg_crc_len);
	for (int i = 0; i <= msg_pad_len - 1; i++)	frame_bytes[i] = msg_pad_data;
	frame_bytes[msg_pad_len] = frame_len;
	frame_bytes[msg_pad_len + 1] = msg_type_para;
	/*for (int i = padding_len + header_len; i <= frame_len - crc_len - 1; i++)
	frame_bytes[i] = *data_pointer++;*/
	memcpy(&frame_bytes[msg_pad_len + msg_hdr_len], data_pointer, data_size);
	crc_res = crc::crc16_ccitt(frame_bytes, frame_len - msg_crc_len, (uint8_t)0x00);
	frame_bytes[frame_len] = (uint8_t)(crc_res & 0xff);//Actually (uint8_t)crc_res; is enough
	frame_bytes[frame_len + 1] = (uint8_t)crc_res >> 8;
	bluetooth_hal::send_arr(frame_bytes, frame_len);
}

void communication::parse_msg(){
	switch (payload_type){
	case command:
		os::issue_command(*payload_data, *(payload_data + 1));
		break;
	case update_ref:

		break;
	case rx_settings://TODO: change name to: Update settings

		break;
	}
}
void communication::send_packet(void* data_pointer, size_t data_size, communication::msg_type_tx msg_type_para){
	main_composer.send_packet(data_pointer, data_size, static_cast<uint8_t>(msg_type_para));
}
void communication::initialize(comm_media comm_media_para){
	used_media = comm_media_para;
	//main_serial.begin(115200);
	if (used_media){
		BT_device.init_bluetooth();
	}
	main_parser.configure(msg_pad_len, msg_hdr_len, msg_crc_len, msg_pad_data);
	main_composer.configure(msg_pad_len, msg_hdr_len, msg_crc_len, msg_pad_data);
}
void communication::send_text(const String &str_para){
	//char* char_text = (char*)&(str_para.toCharArray);
	//main_composer.send_packet((uint8_t*)char_text, sizeof(char_text), text_msg);
	main_serial.println(str_para);
}
void communication::issue_heart_beat(){
	unsigned long current_tick = micros();
	main_composer.send_packet((uint8_t*)(&current_tick), sizeof(current_tick), heart_beat);
}
void communication::stream_data(){
	uint8_t byte_recieved;

#ifndef fake_progress
	while (BT_device.check_for_byte(byte_recieved)){
		if (main_parser.push_byte(byte_recieved, frame_data, frame_len)){//$$$$$$$ Frame Complete $$$$$$$
			payload_data = (uint8_t*)frame_data + msg_pad_len + msg_hdr_len;
			payload_type = (msg_type_rx)*(payload_data - 1);//TODO: Msg_type not payload type
			payload_len = frame_len - msg_pad_len - msg_hdr_len - msg_crc_len;
			parse_msg();
		}
	}
#endif
#ifdef fake_progress
	if (main_serial.available() > 0) {
		// read the incoming byte:
		byte_recieved = main_serial.read();
		if (byte_recieved == 83){		//Corresponds to 'S'
			while (1){
			}
		}
	}
#endif
}
