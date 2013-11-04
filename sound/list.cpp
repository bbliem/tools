#include <iostream>
#include <pulse/pulseaudio.h>

const char* appName = "device-selector";

void sinkInfoListCb(pa_context* context, const pa_sink_info* info, int eol, void* userdata) {
	if(eol) {
		pa_context_disconnect(context);
		return;
	}
	std::cout << "name: " << info->name << '\n';
	std::cout << "description: " << info->description << '\n';
}

void onReady(pa_context* context, void* userdata) {
	pa_context_state_t state = pa_context_get_state(context);
	switch(state) {
		case PA_CONTEXT_FAILED:
		case PA_CONTEXT_TERMINATED:
			exit(1);

		case PA_CONTEXT_READY:
			// Get list of sinks
			pa_context_get_sink_info_list(context, sinkInfoListCb, 0);
			break;

		default:
			break;
	}
}

int main() {
	pa_mainloop* mainloop = pa_mainloop_new();
	pa_mainloop_api* mainloopApi = pa_mainloop_get_api(mainloop);
	pa_context* context = pa_context_new(mainloopApi, appName);

	pa_context_connect(context, 0, PA_CONTEXT_NOFLAGS, 0);

	// Wait for connection to be ready
	pa_context_set_state_callback(context, onReady, 0);

	int retval;
	pa_mainloop_run(mainloop, &retval);

	pa_context_disconnect(context);
	pa_context_unref(context);
	pa_mainloop_free(mainloop);

	return retval;
}
