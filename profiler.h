// profiler.h

#ifndef _PROFILER_h
#define _PROFILER_h
#include "communication.h"
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class profiler{
private:
	PROGMEM static const uint32_t hold_tick_overhead = 12; //in micro seconds
	PROGMEM const float second_to_micros = 1000000.0f;
	PROGMEM static const uint8_t MAX_BUF_SIZE = 60;//i.e: limit the buffer size to 60
	PROGMEM static const uint8_t MAX_PUSH_SIZE = 5;//i.e: the maximum push variable length
	uint32_t start_tick_micros;
	uint8_t profiler_buf[MAX_BUF_SIZE];
	uint8_t profiler_buf_pointer;
	communication local_comm;
public:
	//roll_pv: Roll process variable
	//roll_co: Roll control output
	//ardu_micros, custom_long: uint32_t
	//custom_short1, gyro_roll, acc_x, mag_x: uint16_t
	//roll_pv, roll_co, custom_float1: float
	enum var_type {
		ardu_micros, roll_pv, pitch_pv, yaw_pv, z_pv, x_pv, y_pv, roll_co, pitch_co, yaw_co, z_co, x_co, y_co,
		gyro_roll, gyro_pitch, gyro_yaw, acc_x, acc_y, acc_z, mag_x, mag_y, mag_z, custom_float1, custom_float2,
		custom_float3, custom_float4, custom_long, custom_short1, custom_short2, custom_short3, custom_short4, 
		custom_short5, custom_short6, custom_short7, custom_short8
	};
	void initialize(communication&);
	void probe_variable(var_type type_para, unsigned long value);
	void probe_variable(var_type type_para, uint16_t value);
	void probe_variable(var_type type_para, void* value);
	void start_tick();
	uint32_t get_tick();
	void hold_tick_until(float);
};
#endif

