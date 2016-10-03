// communication.h
#ifndef _COMMUNICATION_h
#define _COMMUNICATION_h

#define main_serial Serial
//#define msg_pad_len 3
//#define msg_hdr_len 2
//#define msg_crc_len 2
//#define msg_pad_data_1 0xFF
//#define msg_pad_data_2 0x00
//#define msg_pad_data_3 0xAA
//#define msg_pad_EOH 0xFF
//#define msg_pad_EOF 0xFF

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
class serial_parser;
class communication; //Prototype for all classes
class serial_composer;
class crc;
class bluetooth_hal;
/* CRC16 implementation acording to CCITT standards
Poly: CCITT-16:   0x1021      =  x16 + x12 + x5 + 1
Checked using: http://depa.usst.edu.cn/chenjq/www2/software/crc/CRC_Javascript/CRCcalculation.htm
*/
const struct tPacketProp{
	uint8_t pad_len = 3;
	uint8_t hdr_len = 2;
	uint8_t crc_len = 2;
	uint8_t pad_data[3] = { 0xFF, 0x00, 0xAA };
	uint8_t pad_EOH = 0xFF;
	uint8_t pad_EOP = 0xFF;
}PacketProp;
class crc{
private:
	static const uint16_t crc16tab[256];
public:
	static unsigned short crc16_ccitt(uint8_t *, int, unsigned short); // crc: crc seed to start with. Use 0x0000
};
class bluetooth_hal{
	//********************************** Important about speed of transmitting ****************************
	//The Tx on Atmega2560 does not have buffer
	//Each framed byte has 10-bits. In 10ms 230bytes are transmitted at 230,020bps
	//*****************************************************************************************************
private:
	void exec_blocking();
public:
	static void send_arr(uint8_t* buf, size_t size);
	bool check_for_byte(uint8_t &byte_recieved);
	static void send_byte(uint8_t byte_para);
	void init_bluetooth();
};
/*TODO: Review this code chunk
void bt_wait_connect(){
bool bt_block = true;
uint8_t read_buff, seq = 0;
while (bt_block == true){
if (main_serial.available() > 0){
read_buff = main_serial.read();
if (read_buff == 'C'){
seq++;
}
else{
seq = 0;
}
}
if (seq == 2){
bt_block = false;
}
}


}*/


class serial_parser
{
private:
	//communication::tPacketProp packet_config;
	int payload_len;
	int padding_len;
	int header_len;
	int crc_len;
	int frame_len;
	int byte_index = 0;
	uint8_t padding;
	uint8_t data_stream[128];
	enum pointer_pose { Padd_search, header_read, EOH_check, msg_read, EOP_check, crc_read, done_reading };
	pointer_pose state = Padd_search;
	tPacketProp packet_config;
	struct tCounters
	{
		int padding_counter;
		int msg_counter;
		int header_counter;
		int crc_counter;
	}byte_counter;
	uint8_t state_machine(byte input_byte);
public:
	//void configure(int para_padding_len, int para_header_len, int para_crc_len, uint8_t para_padding);
	void configure(tPacketProp);
	bool push_byte(uint8_t data, uint8_t* & (frame_data), uint8_t &payload_len_para);
	void get_payload(uint8_t* payload_seq, size_t payload_len);
};


class serial_composer{
private:
	int msg_len;
	int padding_len;
	int header_len;
	int crc_len;
	uint8_t padding;
	uint8_t frame_bytes[100];
	tPacketProp packet_config;
public:
	uint8_t* get_tx_frame_buf();
	//void configure(int para_padding_len, int para_header_len, int para_crc_len, uint8_t para_padding);
	void configure(tPacketProp);
	void send_packet(void* data_pointer, size_t data_size, uint8_t msg_type_para);
};

class communication{
public:
	//struct tPacketProp{
	//	uint8_t pad_len = 3;
	//	uint8_t hdr_len = 2;
	//	uint8_t crc_len = 2;
	//	uint8_t pad_data[3] = { 0xFF, 0x00, 0xAA };
	//	uint8_t pad_EOH = 0xFF;
	//	uint8_t pad_EOP = 0xFF;
	//}; 

private:
	bluetooth_hal BT_device;
	serial_parser main_parser;
	serial_composer main_composer;
	uint8_t* frame_data_ptr;
	uint8_t frame_data[128];
	uint8_t* payload_data;
	uint8_t msg_type;
	uint8_t payload_len;
	size_t frame_len;
	struct tCommand{
		uint8_t commandType;
		uint8_t commandContent[];
	};
	struct tHeader{
		uint16_t padding;
		uint8_t msgLen;
		uint8_t msgType;
	};
	struct tErrorChech{
		uint16_t crc;
	};
	void parse_msg();
public:
	enum msg_type_tx { heart_beat, settings_tx, probe, acknowledgement, msg_tx, rx_values, dbg_data, dbg_txt, chnl_bias, mrft_bag };
	enum msg_type_rx  { command, update_ref, update_settings };
	enum comm_media { USB, BlueTooth };
	enum ACK_type {
		AckSwitchUpdate, AckCtrlSelect,
		AckReferenceSourceChange, AckCommand, AckSettingUpdate
	};
	comm_media used_media;
	void initialize(comm_media comm_media_para);
	void send_text(const String &str_para);
	void issue_heart_beat();
	void stream_data();
	void issue_ack(ACK_type, uint8_t);
	void send_packet(void* data_pointer, size_t data_size, communication::msg_type_tx msg_type_para);
};


#endif

