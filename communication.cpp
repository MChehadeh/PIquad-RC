// 
// 
// 
//TODO: Fix first frame pointer issue
#include "os.h"
#include "communication.h"
#undef dbg_comm
#undef dbg_composer
const uint16_t crc::crc16tab[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};
unsigned short crc::crc16_ccitt(uint8_t *buf, int len, unsigned short crc) // crc: crc seed to start with. Use 0x0000
{
	register int counter;
	for (counter = 0; counter < len; counter++){
		crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ *(uint8_t*)buf++) & 0x00FF];
	}


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

uint8_t* serial_composer::get_tx_frame_buf()//TODO: Revise
{
	return frame_bytes;
}
void serial_composer::configure(tPacketProp packet_config_para)
{
	packet_config = packet_config_para;
}
void serial_composer::send_packet(void* data_pointer, size_t data_size, uint8_t msg_type_para){ //$$$$$$ QC PASS - without CRC $$$$$$
	//SOURCE: for understanding serial timing https://www.baldengineer.com/when-do-you-use-the-arduinos-to-use-serial-flush.html
	uint16_t crc_res;
	uint8_t frame_len = (uint8_t)(packet_config.pad_len + packet_config.hdr_len + data_size + packet_config.crc_len + 2);//+2 for EOH & EOP
	for (int i = 0; i < packet_config.pad_len; i++)
	{
		frame_bytes[i] = packet_config.pad_data[i];
	}
	frame_bytes[packet_config.pad_len] = frame_len;
	frame_bytes[packet_config.pad_len + 1] = msg_type_para;
	frame_bytes[packet_config.pad_len + 2] = packet_config.pad_EOH;
	memcpy(&frame_bytes[packet_config.pad_len + packet_config.hdr_len + 1], data_pointer, data_size);//+1 for EOH
	frame_bytes[frame_len - 3] = packet_config.pad_EOP;
	crc_res = crc::crc16_ccitt(frame_bytes, frame_len - packet_config.crc_len, (uint8_t)0x00);
	frame_bytes[frame_len - 2] = (uint8_t)(crc_res >> 8);
	frame_bytes[frame_len - 1] = (uint8_t)crc_res;
#ifdef dbg_composer
	for (int i = 0; i < frame_len; i++){
		Serial.println(frame_bytes[i]);
	}
#endif
	bluetooth_hal::send_arr(frame_bytes, frame_len);
}

void communication::parse_msg(){
	switch (msg_type){
	case command:
		os::issue_command(*payload_data, *(payload_data + 1), *(payload_data + 2));
		issue_ack(AckCommand, *(payload_data + 3));
		break;
	default:
		//TODO: Handle
		break;
	}
}
void communication::issue_ack(ACK_type ACK_type_para, uint8_t ack_id){
	uint8_t ack_payload[2];
	ack_payload[0] = (uint8_t)ACK_type_para;
	ack_payload[1] = ack_id;
	send_packet((void*)ack_payload, sizeof(ack_payload), acknowledgement);
}
void communication::send_packet(void* data_pointer, size_t data_size, communication::msg_type_tx msg_type_para){
	main_composer.send_packet(data_pointer, data_size, static_cast<uint8_t>(msg_type_para));
}
void communication::initialize(comm_media comm_media_para){
	used_media = comm_media_para;
	//main_serial.begin(115200);
	if (used_media == communication::BlueTooth){
		BT_device.init_bluetooth();
	}
	//PacketProp.crc_len = 2;
	//PacketProp.hdr_len = 2;
	//PacketProp.pad_data[0] = 255;
	//PacketProp.pad_data[1] = 0;
	//PacketProp.pad_data[2] = 170;
	//PacketProp.pad_EOH = 255;
	//PacketProp.pad_EOP = 255;
	//PacketProp.pad_len = 3;
	main_parser.configure(PacketProp);
	main_composer.configure(PacketProp);
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
#ifdef dbg_comm
		Serial.println(byte_recieved);
#endif
		if (main_parser.push_byte(byte_recieved, frame_data_ptr, payload_len)){//$$$$$$$ Frame Complete $$$$$$$
			payload_data = frame_data_ptr + PacketProp.pad_len + PacketProp.hdr_len + 1;// +1 for	EOH
			msg_type = (msg_type_rx)((uint8_t)*(payload_data - 2));//-2 for EOH
			parse_msg();
		}
	}
#endif
#ifdef fake_progress
	if (main_serial.available() > 0) {
		// read the incoming byte:
		byte_recieved = main_serial.read();
		if (byte_recieved == 83){		//Corresponds to 'S'
			wdt_enable(WDTO_15MS);
			while (1){
				//main_serial.println("Stopped");
				motors_hal stopping;
				stopping.writeAllMotors(1064);
			}
		}
	}
#endif
}
//TODO: make CRC more efficient by doing it for payload only
uint8_t serial_parser::state_machine(byte input_byte){  //General Message:  |  Padding. (2-bytes)  |.  Msg. Len. (1-byte)  | Msg. type (1-byte)   |.  Payload (Variable)    |. Crc (2-bytes).   |
	uint8_t return_status = 0;
	switch (state)
	{
	case Padd_search: //$$$$$$$$$$$$$$$$ Reading Padding $$$$$$$$$$$$$$$$$$
		if (input_byte == packet_config.pad_data[byte_counter.padding_counter])
		{
			byte_counter.padding_counter++;
			data_stream[byte_index++] = input_byte;
#ifdef dbg_comm
			Serial.println("padd_rd");
#endif
		}
		else
		{
			byte_counter.padding_counter = 0;
			byte_index = 0; //Packet Faliure
#ifdef dbg_comm
			Serial.println("padd_fail");
#endif
		}
		if (byte_counter.padding_counter == packet_config.pad_len)
		{
			byte_counter.padding_counter = 0;
			state = header_read;
#ifdef dbg_comm
			Serial.println("padd_dn");
#endif
		}
		break;
	case header_read://$$$$$$$$$$$$$$$$$ Reading Header $$$$$$$$$$$$$$$$$$$
		data_stream[byte_index++] = input_byte;
		byte_counter.header_counter++;
		if (byte_counter.header_counter == packet_config.hdr_len)
		{
			byte_counter.header_counter = 0;
			state = EOH_check;
#ifdef dbg_comm
			Serial.println("hdr_rd_dn");
#endif
		}
		else //Because message length is the first byte
		{
			if ((uint16_t)input_byte <= packet_config.hdr_len + packet_config.pad_len + packet_config.crc_len){
				byte_counter.header_counter = 0;
				state = Padd_search;
				byte_index = 0;//Packet Faliure
#ifdef dbg_comm
				Serial.println("len_fail");
				Serial.println((uint16_t)input_byte);
#endif
			}
			else
			{
				payload_len = ((uint16_t)input_byte) - (packet_config.hdr_len + packet_config.pad_len + packet_config.crc_len + 2);//+2 For EOH and EOP
#ifdef dbg_comm
				Serial.println("len_dn");
				Serial.println(payload_len);
#endif
			}
		}
		break;
	case EOH_check:
		if (input_byte == packet_config.pad_EOH){
			data_stream[byte_index++] = input_byte;
			state = msg_read;
		}
		else
		{
			state = Padd_search;
			byte_index = 0;//Packet Faliure
		}
		break;
	case msg_read: //$$$$$$$$$$$$$$$$ Reading the message $$$$$$$$$$$$$$$$$$$$
		data_stream[byte_index++] = input_byte;
		byte_counter.msg_counter++;
#ifdef dbg_comm
		Serial.println("msg_rd");
#endif
		if (byte_counter.msg_counter == payload_len)
		{
			byte_counter.msg_counter = 0;
			state = EOP_check;
#ifdef dbg_comm
			Serial.println("msg_rd_dn");
#endif
		}
		break;
	case EOP_check:
		if (input_byte == packet_config.pad_EOP){
			data_stream[byte_index++] = input_byte;
			state = crc_read;
		}
		else
		{
			state = Padd_search;
			byte_index = 0;//Packet Faliure
		}
		break;
	case crc_read://$$$$$$$$$$$$$$$$$ Reading the CRC $$$$$$$$$$$$$$$$$$$$$$
		data_stream[byte_index++] = input_byte;
		byte_counter.crc_counter++;
#ifdef dbg_comm
		Serial.println("crc_rd");
#endif
		if (byte_counter.crc_counter == packet_config.crc_len)
		{
			byte_counter.crc_counter = 0;
			state = done_reading;
#ifdef dbg_comm
			Serial.println("crc_rd_dn");
#endif
		}
		break;
	}
	if (state == done_reading){
		uint16_t crc_res = crc::crc16_ccitt(data_stream, byte_index, 0);
		if (crc_res == 0) {
			return_status = 1;
#ifdef dbg_comm
			Serial.println("crc_ok");
			Serial.println((uint8_t)(crc_res >> 8));
			Serial.println((uint8_t)crc_res);
#endif
		}
		else{
#ifdef dbg_comm
			Serial.println("crc_err");
			Serial.println((uint8_t)(crc_res >> 8));
			Serial.println((uint8_t)crc_res);
			Serial.println(byte_index);
#endif
		}
		state = Padd_search;
		byte_index = 0;
#ifdef dbg_comm
		Serial.println("msg_dn");
#endif
	}
#ifdef dbg_comm
	Serial.println(byte_index);
#endif
	return return_status;
}
void serial_parser::configure(tPacketProp packet_config_para)
{
	packet_config = packet_config_para;
	byte_index = 0;
#ifdef dbg_comm
	Serial.println("Packet:");
	Serial.println(packet_config.pad_data[0]);
	Serial.println(packet_config.pad_data[1]);
	Serial.println(packet_config.pad_data[2]);
	Serial.println(packet_config.pad_EOH);
	Serial.println(packet_config.pad_EOP);
	Serial.println(packet_config.crc_len);
	Serial.println(packet_config.pad_len);
#endif

}
bool serial_parser::push_byte(uint8_t data, uint8_t* &frame_data, uint8_t &payload_len_para)
{
	if (state_machine(data) == 1)
	{
		payload_len_para = payload_len;
		frame_data = data_stream;
		return true;
	}
	else
	{
		return false;
	}
}
void serial_parser::get_payload(uint8_t* payload_seq, size_t payload_len){
	//TODO: 
}


