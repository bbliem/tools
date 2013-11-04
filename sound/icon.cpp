#include <iostream>
#include <vector>
#include <pulse/pulseaudio.h>
#include <pulse/ext-stream-restore.h>
#include <pulse/glib-mainloop.h>
#include <gtk/gtk.h>

const char* appName = "device-selector";

struct Sink {
	std::string name;
	std::string description;
	Sink(const std::string& name, const std::string& description) {
		this->name = name;
		this->description = description;
	}
};
std::vector<Sink> sinks;
pa_context* _context;

void onExtStreamRestoreRead(pa_context* context, const pa_ext_stream_restore_info *info, int eol, void* userdata) {
	if(eol) {
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
void sinkInfoListCb(pa_context* context, const pa_sink_info* info, int eol, void* userdata) {
	if(eol) {
		return;
	}
	std::cout << "name: " << info->name << '\n';
	std::cout << "description: " << info->description << '\n';
	sinks.push_back(Sink(info->name, info->description));
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

void onActivate(GtkMenuItem* item, gpointer userdata) {
	const char* device = reinterpret_cast<const char*>(userdata);
	std::cout << device << '\n';

	// Set sink
	pa_operation* op = pa_context_set_default_sink(_context, device, 0, 0);
	if(!op)
		std::cerr << "FAIL! " << pa_strerror(pa_context_errno(_context)) << '\n';
	pa_operation_unref(op);

	// Get all streams to then move them to new device
	op = pa_ext_stream_restore_read(_context, onExtStreamRestoreRead, userdata);
	if(!op)
		std::cerr << "FAIL! " << pa_strerror(pa_context_errno(_context)) << '\n';
	pa_operation_unref(op);
}

void showMenu(GtkStatusIcon* statusIcon, gpointer userdata, guint button = 1, guint activate_time = 0) {
	if(activate_time == 0)
		activate_time = gtk_get_current_event_time();

	GSList* group = 0;
	GtkWidget* item;
	gint i;
	for (i = 0; i < 5; i++)
	{
		item = gtk_radio_menu_item_new_with_label (group, "This is an example");
		group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (item));
		if (i == 1)
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (item), TRUE);
	}

	GtkWidget* menu = gtk_menu_new();

	for(std::vector<Sink>::const_iterator it = sinks.begin(); it != sinks.end(); ++it) {
		GtkWidget* item = gtk_radio_menu_item_new_with_label(group, it->description.c_str());
		group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (item));
		if (i == 1)
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (item), TRUE);

		g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(onActivate), (void*)it->name.c_str());
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	}

	gtk_widget_show_all(menu);
	gtk_menu_popup(GTK_MENU(menu), 0, 0, gtk_status_icon_position_menu, statusIcon, button, activate_time);
}

void onIconActivate(GtkStatusIcon* statusIcon, gpointer userdata) {
	showMenu(statusIcon, userdata);
}

void onPopupMenu(GtkStatusIcon* statusIcon, guint button, guint activate_time, gpointer userdata) {
	showMenu(statusIcon, userdata, button, activate_time);
}

int main(int argc, char** argv) {
	gtk_init(&argc, &argv);

	GMainLoop* mainloop = g_main_loop_new(0, 0);

	GtkStatusIcon* icon = gtk_status_icon_new();

	pa_glib_mainloop* paGlibMainloop = pa_glib_mainloop_new(g_main_loop_get_context(mainloop));

	g_signal_connect(G_OBJECT(icon), "activate", G_CALLBACK(onIconActivate), NULL);
	g_signal_connect(G_OBJECT(icon), "popup-menu", G_CALLBACK(onPopupMenu), NULL);

	gtk_status_icon_set_from_icon_name(icon, GTK_STOCK_INDEX);
	gtk_status_icon_set_tooltip_text(icon, "Audio output device changer");
	gtk_status_icon_set_visible(icon, TRUE);

	pa_mainloop_api* paMainloopApi = pa_glib_mainloop_get_api(paGlibMainloop);
	pa_context* context = pa_context_new(paMainloopApi, appName);
	_context = context;
	pa_context_connect(context, 0, PA_CONTEXT_NOFLAGS, 0);

	// Wait for connection to be ready
	pa_context_set_state_callback(context, onReady, 0);

	g_main_loop_run(mainloop);

	pa_context_disconnect(context);
	pa_context_unref(context);

	pa_context_disconnect(context);
	pa_context_unref(context);
	pa_glib_mainloop_free(paGlibMainloop);
	g_main_loop_unref(mainloop);

	return 0;
}
