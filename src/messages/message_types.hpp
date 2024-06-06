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