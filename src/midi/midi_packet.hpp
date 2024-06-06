#pragma once

struct MIDIPacket
{
    MIDICin cin;
    uint8_t cable;
    uint8_t data0;
    uint8_t data1;

    MIDIPacket() = default;

    MIDIPacket(const uint8_t packet[4])
    {
        if (packet[1] < 0xF0)
        {
            cin = static_cast<MIDICin>(packet[1] & 0xF0);
            cable = static_cast<uint8_t>(packet[1] & 0x0F);
            data0 = packet[2];
            data1 = packet[3];
        }
        else
        {
            cin = static_cast<MIDICin>(packet[1]);
            data0 = packet[2];
            data1 = packet[3];
        }
    }
};