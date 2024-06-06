#pragma once

enum class MIDICin : uint8_t
{
    note_off = 0x80,
    note_on = 0x90,
    aftertouch = 0xA0,
    control_change = 0xB0,
    program_change = 0xC0,
    pitch_bend_change = 0xE0,
    sys_exclusive = 0xF0,
    sys_time_code = 0xF1,
    sys_song_position = 0xF2,
    sys_song_select = 0xF3,
    sys_tune_request = 0xF6,
    rt_clock = 0xF8,
    rt_start = 0xFA,
    rt_continue = 0xFB,
    rt_stop = 0xFC,
    rt_sense = 0xFE, // active sensing
    rt_reset = 0xFF,
};

constexpr const char *get_midi_cin_name(MIDICin cin)
{
    switch (cin)
    {
    case MIDICin::note_off:
        return "note_off";
    case MIDICin::note_on:
        return "note_on";
    case MIDICin::aftertouch:
        return "aftertouch";
    case MIDICin::control_change:
        return "control_change";
    case MIDICin::program_change:
        return "program_change";
    case MIDICin::pitch_bend_change:
        return "pitch_bend_change";
    case MIDICin::sys_exclusive:
        return "system_exclusive";
    case MIDICin::sys_time_code:
        return "sys_time_code";
    case MIDICin::sys_song_position:
        return "sys_song_position";
    case MIDICin::sys_song_select:
        return "sys_song_select";
    case MIDICin::sys_tune_request:
        return "sys_tune_request";
    case MIDICin::rt_clock:
        return "rt_clock";
    case MIDICin::rt_start:
        return "rt_start";
    case MIDICin::rt_continue:
        return "rt_continue";
    case MIDICin::rt_stop:
        return "rt_stop";
    case MIDICin::rt_sense:
        return "rt_sense";
    case MIDICin::rt_reset:
        return "rt_reset";
    }

    return "unknown";
}