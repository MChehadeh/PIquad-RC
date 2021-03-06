#include "os.h"
communication os::comm_channel = comm_channel;
profiler os::main_profiler;
struct os::tStartup_flags os::startup_flags;
enum os::scenario_t os::active_scenario;
bool os::boot(){
	//################### Initialize the communication object ###################
	comm_channel.initialize(communication::USB);
	//################### Initialize Rx ###################
	RX::configureReceiver();
	//################### Startup process ###################
	while (run_startup_process() != os::done){
		1;
	}
	return true;
}
os::startup_state os::run_startup_process(){ //################### Startup state machine ###################
	static startup_state current_startup_state = wait_for_heartbeat_request_I;//This is valid as long as we have one instance of os
	comm_channel.stream_data();//{current_startup_state}
	switch (current_startup_state)	{
	case os::wait_for_heartbeat_request_I:
#ifdef fake_progress
		issue_command(request_heart_beat, 0); //!!!!!!!!!!!!!!!!! Fake Message !!!!!!!!!!!!!!!!!!!!!!!!
#endif
		if (startup_flags.heartbeat_requested_flag){
			current_startup_state = wait_for_start_command;
		}
		break;
	case os::wait_for_start_command:
#ifdef fake_progress
		issue_command(start, 0); //!!!!!!!!!!!!!!!!! Fake Message !!!!!!!!!!!!!!!!!!!!!!!!
#endif
		if (startup_flags.start_command_flag){
			current_startup_state = done;
		}
		break;
	case os::done:
		break;
	default:
		1;
		break;
	}
	return current_startup_state;
}

void os::issue_command(uint8_t cmd_type_para, uint8_t cmd_data, uint8_t cmd_chnl){
	switch ((cmd_type)cmd_type_para)
	{
	case (os::arm) :
		startup_flags.start_command_flag = 1;
		break;
	case (os::request_heart_beat) :
		//comm_channel.issue_heart_beat();
		startup_flags.heartbeat_requested_flag = 1;
		break;
	default:
		1;
		break;
	}
}
void os::perform_loop(){
	main_profiler.start_tick();
	comm_channel.stream_data();
	RX::computeRC();
	float test_tick;
	static uint32_t test_counter = 0;
	test_counter++;
	main_profiler.probe_variable(main_profiler.custom_short1, RX::rcValue[0]);
	main_profiler.probe_variable(main_profiler.custom_short2, RX::rcValue[1]);
	main_profiler.probe_variable(main_profiler.custom_short3, RX::rcValue[2]);
	main_profiler.probe_variable(main_profiler.custom_short4, RX::rcValue[3]);
	//******************** Use this to check how much time a loop is taking *******************//
	/*uint32_t max_dt_check = main_profiler.get_tick();
	main_profiler.probe_variable(main_profiler.custom_long, max_dt_check);*/

	main_profiler.hold_tick_until(dt_manual);
}
