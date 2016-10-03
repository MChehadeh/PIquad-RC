// os.h

#ifndef _OS_h
#define _OS_h
#include "RX.h"
#include "profiler.h"
#include "communication.h"
#define dt_manual 0.1f
#undef fake_progress
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
//############################# Forward declared dependency ###################################
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Important things missing (revise) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
-> Set auxillary thrust for pitch setup (os::boot)

*/



class os{
	enum ctrl_channel { roll, pitch, yaw, z_axis, x_axis, y_axis, last_channel = y_axis };
	enum cmd_type  { arm, disarm, update_ref_src, sw_change, mrft_select, calibrate_sensors, request_heart_beat, update_EEPROM, get_bias, engage_motors, disengage_motors };
	static communication comm_channel;
	static profiler main_profiler;
	enum startup_state {
		wait_for_heartbeat_request_I, wait_for_test_plan, wait_for_calibration_cmd,
		wait_for_controller_parameters, wait_for_start_command, done
	};
	struct tStartup_flags{
		bool heartbeat_requested_flag;
		bool calibration_done_flag;
		bool controller_parameters_ready_flag;
		bool test_plan_updated_flag;
		bool start_command_flag;
	};
	static tStartup_flags startup_flags;
public:
	enum scenario_t { full, thr_setup, disable_mtrs, sensors_cal, pitch_setup, yaw_setup };
	static scenario_t active_scenario;
	enum ctrl_channel { roll, pitch, yaw, z_axis, x_axis, y_axis, last_channel = y_axis };
	startup_state run_startup_process();
	bool boot();
	static void issue_command(uint8_t, uint8_t,uint8_t);
	void perform_loop();
};


#endif

