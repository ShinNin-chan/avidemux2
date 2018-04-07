// automatically generated by admSerialization.py do not edit
#include "string"
#pragma once
typedef struct {
struct  {
	uint32_t apiVersion;
}version;
struct  {
	bool swap_if_A_greater_than_B;
	bool saveprefsonexit;
	bool ignoresavedmarkers;
	bool reuse_2pass_log;
	bool audiobar_uses_master;
	uint32_t threading_lavc;
	uint32_t cpu_caps;
	bool mpeg_no_limit;
	bool alternate_mp3_tag;
	bool dxva2;
	bool vdpau;
	bool xvba;
	bool libva;
	bool videotoolbox;
	bool enable_opengl;
	bool cap_refresh_enabled;
	uint32_t cap_refresh_value;
	std::string sdlDriver;
	bool use_last_read_dir_as_target;
	bool copy_mode_sanitize_dts;
}features;
struct  {
	bool use_alternate_kbd_shortcuts;
	bool swap_up_down_keys;
	std::string alt_mark_a;
	std::string alt_mark_b;
	std::string alt_reset_markers;
	std::string alt_goto_mark_a;
	std::string alt_goto_mark_b;
	std::string alt_begin;
	std::string alt_end;
}keyboard_shortcuts;
struct  {
	bool enabled;
	uint32_t lastCheck;
}update;
struct  {
	std::string lastdir_read;
	std::string lastdir_write;
	std::string file1;
	std::string file2;
	std::string file3;
	std::string file4;
}lastfiles;
struct  {
	std::string lastdir_read;
	std::string lastdir_write;
	std::string file1;
	std::string file2;
	std::string file3;
	std::string file4;
}lastprojects;
uint32_t message_level;
struct  {
	std::string audiodevice;
	std::string alsa_device;
}audio_device;
uint32_t videodevice;
struct  {
	uint32_t encoding;
	uint32_t indexing;
	uint32_t playback;
}priority;
struct  {
	uint32_t postproc_type;
	uint32_t postproc_value;
	uint32_t downmixing;
	std::string language;
	bool warn_for_fonts;
}Default;
struct  {
	bool avisynth_always_ask;
	uint32_t avisynth_defaultport;
	uint32_t avisynth_localport;
}avisynth;
bool reset_encoder_on_video_load;
}my_prefs_struct;
