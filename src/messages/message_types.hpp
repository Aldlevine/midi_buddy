#pragma once

struct ClockMessage
{
    uint32_t clock;
};

struct NoteMessage
{
    uint8_t note;
    bool on;
};

struct ControlMessage
{
    uint8_t control_id;
    uint8_t value;
};