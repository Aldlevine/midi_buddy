#include "pico/unique_id.h"
#include "bsp/board.h"
#include "tusb.h"

enum class ITF : uint8_t
{
    num_midi = 0,
    num_midi_streaming,
    num_total,
};

enum class STRID : uint8_t
{
    langid = 0,
    manufacturer,
    product,
    serial,
};

#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
constexpr auto usb_pid = (0x4000 |
                          _PID_MAP(CDC, 0) |
                          _PID_MAP(MSC, 1) |
                          _PID_MAP(HID, 2) |
                          _PID_MAP(MIDI, 3) |
                          _PID_MAP(VENDOR, 4));
#undef _PID_MAP

// constexpr auto usb_vid = 0x12a7;
constexpr auto usb_vid = 0x7175;
constexpr auto usb_bcd = 0x0200;

tusb_desc_device_t const desc_device{
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = usb_bcd,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = usb_vid,
    .idProduct = usb_pid,
    .bcdDevice = 0x0100,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01,
};

constexpr auto config_total_len = TUD_CONFIG_DESC_LEN + TUD_MIDI_DESC_LEN;

#if CFG_TUSB_MCU == OPT_MCU_LPC175X_6X || CFG_TUSB_MCU == OPT_MCU_LPC177X_8X || CFG_TUSB_MCU == OPT_MCU_LPC40XX
// LPC 17xx and 40xx endpoint type (bulk/interrupt/iso) are fixed by its number
// 0 control, 1 In, 2 Bulk, 3 Iso, 4 In etc ...
constexpr auto epnum_midi_out = 0x02;
constexpr auto epnum_midi_in = 0x02;
#elif CFG_TUSB_MCU == OPT_MCU_FT90X || CFG_TUSB_MCU == OPT_MCU_FT93X
// On Bridgetek FT9xx endpoint numbers must be unique...
constexpr auto epnum_midi_out = 0x02;
constexpr auto epnum_midi_in = 0x03;
#else
constexpr auto epnum_midi_out = 0x01;
constexpr auto epnum_midi_in = 0x01;
#endif

constexpr uint8_t desc_fs_configuration[]{
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, static_cast<uint8_t>(ITF::num_total), 0, config_total_len, 0x00, 100),
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MIDI_DESCRIPTOR(static_cast<uint8_t>(ITF::num_midi), 0, epnum_midi_out, (0x80 | epnum_midi_in), 64),
};

char serial[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];

constexpr const char *string_desc_array[]{
    (const char[]){0x09, 0x04}, // 0: English (0x0409)
    "XneyZnek",                 // 1: Manufacturer
    "Analog Buddy",             // 2: Product
    serial,                     // 3: Serial use flash id
};

#if TUD_OPT_HIGH_SPEED
constexpr uint8_t desc_hs_configuration[]{
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, static_cast<uint8_t>(ITF::num_total), 0, config_total_len, 0x00, 100),
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MIDI_DESCRIPTOR(static_cast<uint8_t>(ITF::num_midi), 0, epnum_midi_out, (0x80 | epnum_midi_in), 512),
};
#endif

extern "C"
{
    const uint8_t *tud_descriptor_device_cb()
    {
        return reinterpret_cast<const uint8_t *>(&desc_device);
    }

    const uint8_t *tud_descriptor_configuration_cb(uint8_t index)
    {
        (void)index;

#if TUD_OPT_HIGH_SPEED
        if (tud_speed_get() == TUSB_SPEED_HIGH)
        {
            return desc_hs_configuration;
        }
#endif
        return desc_fs_configuration;
    }

    const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t lang)
    {
        static uint16_t desc_str[32 + 1];
        static char serial[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];

        (void)lang;
        size_t chr_count;

        switch (static_cast<STRID>(index))
        {
        case STRID::langid:
            memccpy(&desc_str[1], string_desc_array[0], 2, 1);
            chr_count = 1;
            break;

        case STRID::serial:
            pico_get_unique_board_id_string(serial, sizeof(serial));
            chr_count = strlen(serial);
            break;

        default:
            if (index >= sizeof(string_desc_array) / sizeof(string_desc_array[0]))
            {
                return nullptr;
            }

            const char *str = string_desc_array[index];
            chr_count = strlen(str);
            const size_t max_count = sizeof(desc_str) / sizeof(desc_str[0]) - 1;
            if (chr_count > max_count)
            {
                chr_count = max_count;
            }

            for (size_t i = 0; i < chr_count; ++i)
            {
                desc_str[1 + i] = str[i];
            }
            break;
        }

        desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | 2 * chr_count + 2);

        return desc_str;
    }
}
