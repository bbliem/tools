#include <iostream>
#include <pulse/pulseaudio.h>
#include <pulse/ext-stream-restore.h>

const char* appName = "device-selector";

void onExtStreamRestoreRead(pa_context* context, const pa_ext_stream_restore_info *info, int eol, void* userdata) {
	if(eol) {
		pa_context_disconnect(context);
		return;
	}

	const char* newDevice = reinterpret_cast<const char*>(userdata);

	std::cout << "stream name: " << info->name << '\n';
	std::cout << "stream device: " << info->device << '\n';
	// Change sink of this stream
	pa_ext_stream_restore_info newInfo;
	newInfo.name = info->name;
	newInfo.channel_map = info->channel_map;
	newInfo.volume = info->volume;
	newInfo.mute = info->mute;
	newInfo.device = newDevice;

	pa_operation* op = pa_ext_stream_restore_write(context, PA_UPDATE_REPLACE, &newInfo, 1, 1, 0, 0);
	if(!op)
		std::cerr << "FAIL! " << pa_strerror(pa_context_errno(context)) << '\n';
	pa_operation_unref(op);
}

void onReady(pa_context* context, void* userdata) {
	const char* newDevice = reinterpret_cast<const char*>(userdata);

	pa_context_state_t state = pa_context_get_state(context);
	switch(state) {
		case PA_CONTEXT_FAILED:
		case PA_CONTEXT_TERMINATED:
			exit(1);

		case PA_CONTEXT_READY:
			{
			// Set sink
			pa_operation* op = pa_context_set_default_sink(context, newDevice, 0, 0);
			if(!op)
				std::cerr << "FAIL! " << pa_strerror(pa_context_errno(context)) << '\n';
			pa_operation_unref(op);

			// Get all streams to then move them to new device
			op = pa_ext_stream_restore_read(context, onExtStreamRestoreRead, userdata);
			if(!op)
				std::cerr << "FAIL! " << pa_strerror(pa_context_errno(context)) << '\n';
			pa_operation_unref(op);
			}
			break;

		default:
			break;
	}
}

int main(int argc, char** argv) {
	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << " sink_name\n";
		return 1;
	}

	pa_mainloop* mainloop = pa_mainloop_new();
	pa_mainloop_api* mainloopApi = pa_mainloop_get_api(mainloop);
	pa_context* context = pa_context_new(mainloopApi, appName);

	pa_context_connect(context, 0, PA_CONTEXT_NOFLAGS, 0);

	// Wait for connection to be ready
	pa_context_set_state_callback(context, onReady, argv[1]);

	int retval;
	pa_mainloop_run(mainloop, &retval);

	pa_context_disconnect(context);
	pa_context_unref(context);
	pa_mainloop_free(mainloop);

	return retval;
}
