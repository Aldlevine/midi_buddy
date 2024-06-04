#pragma once

constexpr auto CLOCKS_PER_BEAT = 24;        // 1/4 note
constexpr auto CLOCKS_PER_HALF_BEAT = 12;   // 1/8 note
constexpr auto CLOCKS_PER_QUARTER_BEAT = 6; // 1/16 note
constexpr auto CLOCKS_PER_EIGHTH_BEAT = 3;  // 1/32 note

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

struct MIDIPacket
{
    MIDICin cin;
    uint8_t cable;
    uint8_t data0;
    uint8_t data1;
};

MIDIPacket process_midi_packet(uint8_t packet[4])
{
    if (packet[1] < 0xF0)
    {
        return MIDIPacket{
            .cin = static_cast<MIDICin>(packet[1] & 0xF0),
            .cable = static_cast<uint8_t>(packet[1] & 0x0F),
            .data0 = packet[2],
            .data1 = packet[3],
        };
    }
    return MIDIPacket{
        .cin = static_cast<MIDICin>(packet[1]),
        .data0 = packet[2],
        .data1 = packet[3],
    };
}

uint16_t combine_14b(uint8_t first, uint8_t second)
{
    uint16_t result;
    result = second;
    result <<= 7;
    result |= first;
    return result;
}