#pragma once

#include "display/channel_display.hpp"
#include "messages/messages.hpp"
#include "midi/midi.hpp"
#include "note_tracker/note_tracker.hpp"

#include <set>

constexpr uint8_t KEY_HEIGHT = CHANNEL_HEIGHT / 8;

struct Rect
{
    uint8_t x_min;
    uint8_t y_min;
    uint8_t x_max;
    uint8_t y_max;
};

const std::set<uint8_t> WHITE_KEYS{0, 2, 4, 5, 7, 9, 11};
const std::set<uint8_t> BLACK_KEYS{1, 3, 6, 8, 10};

class NoteChannelDisplay : public virtual ChannelDisplay
{
    NoteTracker &note_tracker;

public:
    NoteChannelDisplay(NoteTracker &note_tracker) : note_tracker{note_tracker} {}

    ~NoteChannelDisplay() override
    {
    }

    void draw_channel(uint8_t channel, DisplayOutput &output) override
    {
        bool audible = true;

        {
            size_t i = 0;
            for (auto note : note_tracker.voices())
            {
                auto key = note % 12;
                if (WHITE_KEYS.find(key) != WHITE_KEYS.end())
                {
                    fill_key(output, channel, key, i == 0);
                }

                ++i;
            }
        }

        draw_keyboard(output, channel);

        {
            size_t i = 0;
            for (auto note : note_tracker.voices())
            {
                auto key = note % 12;
                if (BLACK_KEYS.find(key) != BLACK_KEYS.end())
                {
                    fill_key(output, channel, key, i == 0);
                }

                ++i;
            }
        }
    }

    Rect get_key_rect(uint8_t channel, uint8_t key)
    {
        uint8_t cx = channel * CHANNEL_WIDTH;
        uint8_t cx_mid = cx + CHANNEL_WIDTH / 2;
        uint8_t cx_end = cx + CHANNEL_WIDTH;
        uint8_t cy = CHANNEL_HEIGHT - 4;

        // white keys
        if (WHITE_KEYS.find(key) != WHITE_KEYS.end())
        {
            constexpr uint8_t KEY_OFFSET_WHITE[] = {0, 0, 1, 0, 2, 3, 0, 4, 0, 5, 0, 6};
            auto key_offset = KEY_OFFSET_WHITE[key];
            return Rect{
                cx,
                (uint8_t)(cy - (KEY_HEIGHT * (key_offset + 1))),
                cx_end,
                (uint8_t)(cy - (KEY_HEIGHT * key_offset)),
            };
        }

        // black keys
        switch (key)
        {
        case 1:
            return Rect{cx, uint8_t(cy - (KEY_HEIGHT * 4) / 3), cx_mid, uint8_t(cy - (KEY_HEIGHT * 2) / 3)};
        case 3:
            return Rect{cx, uint8_t(cy - (KEY_HEIGHT * 7) / 3), cx_mid, uint8_t(cy - (KEY_HEIGHT * 5) / 3)};
        case 6:
            return Rect{cx, uint8_t(cy - (KEY_HEIGHT * 13) / 3), cx_mid, uint8_t(cy - (KEY_HEIGHT * 11) / 3)};
        case 8:
            return Rect{cx, uint8_t(cy - (KEY_HEIGHT * 16) / 3), cx_mid, uint8_t(cy - (KEY_HEIGHT * 14) / 3)};
        case 10:
            return Rect{cx, uint8_t(cy - (KEY_HEIGHT * 19) / 3), cx_mid, uint8_t(cy - (KEY_HEIGHT * 17) / 3)};
        }

        return Rect{};
    }

    void draw_keyboard(DisplayOutput &output, uint8_t channel)
    {
        for (auto key : WHITE_KEYS)
        {
            auto rect = get_key_rect(channel, key);
            output.draw_rect(rect.x_min, rect.y_min, rect.x_max, rect.y_max);
        }

        for (auto key : BLACK_KEYS)
        {
            auto rect = get_key_rect(channel, key);
            output.fill_rect(rect.x_min, rect.y_min, rect.x_max, rect.y_max, 0, pico_ssd1306::WriteMode::SUBTRACT);
            output.draw_rect(rect.x_min, rect.y_min, rect.x_max, rect.y_max);
        }
    }

    void fill_key(DisplayOutput output, uint8_t channel, uint8_t key, bool audible)
    {
        auto rect = get_key_rect(channel, key);
        output.fill_rect(rect.x_min, rect.y_min, rect.x_max, rect.y_max, audible ? 0 : 8);
    }
};