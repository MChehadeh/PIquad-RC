// 
// 
// 

#include "profiler.h"

void profiler::initialize(communication& main_comm){//Pass by reference, look at the example at the end of this file.
	local_comm = main_comm;
}
void profiler::probe_variable(var_type type_para, uint16_t value){
	if (profiler_buf_pointer < (MAX_BUF_SIZE - MAX_PUSH_SIZE)){
		profiler_buf[profiler_buf_pointer++] = (uint8_t)type_para;
		profiler_buf[profiler_buf_pointer++] = (uint8_t)value; //The low byte is pushed first -little endian
		profiler_buf[profiler_buf_pointer++] = (uint8_t)value >> 8;
	}
}
void profiler::probe_variable(var_type type_para, unsigned long value){
	if (profiler_buf_pointer < (MAX_BUF_SIZE - MAX_PUSH_SIZE)){
		profiler_buf[profiler_buf_pointer++] = (uint8_t)type_para;
		profiler_buf[profiler_buf_pointer++] = (uint8_t)value; //The lowest byte is pushed first
		profiler_buf[profiler_buf_pointer++] = (uint8_t)(value >> 8);
		profiler_buf[profiler_buf_pointer++] = (uint8_t)(value >> 16);
		profiler_buf[profiler_buf_pointer++] = (uint8_t)(value >> 24);
	}
}
void profiler::probe_variable(var_type type_para, void* value){
	uint8_t* data_pointer;
	data_pointer = (uint8_t*)value;
	if (profiler_buf_pointer < (MAX_BUF_SIZE - MAX_PUSH_SIZE)){
		profiler_buf[profiler_buf_pointer++] = (uint8_t)type_para;
		profiler_buf[profiler_buf_pointer++] = (uint8_t)(*(data_pointer++)); //The lowest byte is pushed first
		profiler_buf[profiler_buf_pointer++] = (uint8_t)(*(data_pointer++));
		profiler_buf[profiler_buf_pointer++] = (uint8_t)(*(data_pointer++));
		profiler_buf[profiler_buf_pointer++] = (uint8_t)(*(data_pointer++));
	}
}
void profiler::start_tick(){
	start_tick_micros = micros();
	profiler_buf_pointer = 0;
}
uint32_t profiler::get_tick(){
	return (micros() - start_tick_micros);
}
void profiler::hold_tick_until(float tick_duration){
	local_comm.send_packet(profiler_buf, profiler_buf_pointer, local_comm.rx_values);
	while ((micros() - start_tick_micros - hold_tick_overhead) <= ((uint32_t)(second_to_micros*tick_duration))){
	}
}
//************************* Example program: Pass object by reference *******************************//
/*
#include <iostream>
#include <string>
using namespace std;
class foo{
public:
float x;
void init(){
x = 10;
}
};
class foo_get{
public:
foo local_foo;
void init(foo& foo_para){
local_foo = foo_para;
}
float get_val(){
return local_foo.x;
}
};
int main()
{
foo main_foo;
main_foo.init();
foo_get main_foo_get;
main_foo_get.init(main_foo);
cout << main_foo_get.get_val();
}
*/