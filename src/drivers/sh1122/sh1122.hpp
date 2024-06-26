#pragma once

#include "graphics/graphics.hpp"

#include <hardware/dma.h>
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <pico/stdlib.h>

#include <array>

enum class SH1122Commands : uint8_t
{
    display_off = 0xAE,
    display_on = 0xAF,
    set_display_clock_div = 0xD5,
    set_multiplex = 0xA8,
    set_display_offset = 0xD3,
    set_start_line = 0x40,
    charge_pump = 0xAD,
    memory_mode = 0xA0,
    segment_remap = 0xA1,
    com_scan_dir = 0xC8,
    set_contrast = 0x81,
    set_precharge = 0xD9,
    set_vcom_deselect = 0xDB,
    set_vsegm = 0xDC,
    set_discharge_vsl = 0x30,
    display_all_on_resume = 0xA4,
    normal_display = 0xA6,
};

constexpr auto operator+(const SH1122Commands cmd) noexcept
{
    return static_cast<uint8_t>(cmd);
}

template <
    size_t width_,
    size_t height_,
    uint8_t sck_,
    uint8_t mosi_,
    uint8_t dc_,
    uint8_t cs_,
    uint8_t rst_>
class SH1122Driver
{
    static constexpr auto width = width_;
    static constexpr auto height = height_;
    static constexpr auto sck = sck_;
    static constexpr auto mosi = mosi_;
    static constexpr auto dc = dc_;
    static constexpr auto cs = cs_;
    static constexpr auto rst = rst_;

    spi_inst *spi;
    uint bd;

public:
    using frame_buffer_t = Image<ImageFormat::GS4_HMSB, width, height>;

    std::array<frame_buffer_t, 2> frame_buffers;
    size_t front_buffer_idx = 0;
    uint8_t dma_channel = 0;
    dma_channel_config dma_config;

    SH1122Driver(spi_inst *spi)
        : spi{spi}
    {
    }

    void init()
    {

        spi_init(spi, 32500000);
        spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

        dma_channel = dma_claim_unused_channel(true);
        dma_config = dma_channel_get_default_config(dma_channel);
        channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_8);
        channel_config_set_dreq(&dma_config, spi_get_dreq(spi, true));

        gpio_init(dc);
        gpio_init(cs);
        gpio_init(rst);
        gpio_set_dir(dc, GPIO_OUT);
        gpio_set_dir(cs, GPIO_OUT);
        gpio_set_dir(rst, GPIO_OUT);

        gpio_set_function(sck, GPIO_FUNC_SPI);
        gpio_set_function(mosi, GPIO_FUNC_SPI);

        gpio_put(rst, 1);

        select_device();
        set_cmd_mode();
        write_cmd(+SH1122Commands::display_off);
        write_cmd(+SH1122Commands::set_display_clock_div, 0xF0);
        write_cmd(+SH1122Commands::set_display_offset, 0x20);
        write_cmd(+SH1122Commands::set_start_line);
        write_cmd(+SH1122Commands::charge_pump, 0x8F);
        write_cmd(+SH1122Commands::segment_remap);
        write_cmd(+SH1122Commands::com_scan_dir);
        write_cmd(+SH1122Commands::set_contrast, 0x44);
        write_cmd(+SH1122Commands::set_multiplex, 0x3F);
        write_cmd(+SH1122Commands::set_precharge, 0x81);
        write_cmd(+SH1122Commands::set_vcom_deselect, 0x00);
        write_cmd(+SH1122Commands::set_vsegm, 0x00);
        write_cmd(+SH1122Commands::set_discharge_vsl + 0x06);
        write_cmd(+SH1122Commands::display_all_on_resume);
        write_cmd(+SH1122Commands::normal_display);
        write_cmd(+SH1122Commands::display_on);

        deselect_device();
    }

    frame_buffer_t &get_front_buffer()
    {
        return frame_buffers[front_buffer_idx];
    }

    frame_buffer_t &get_back_buffer()
    {
        return frame_buffers[(front_buffer_idx + 1) % frame_buffers.size()];
    }

    void swap_buffers()
    {
        if (dma_channel_is_busy(dma_channel))
        {
            dma_channel_wait_for_finish_blocking(dma_channel);
        }

        deselect_device();
        select_device();

        set_cmd_mode();
        write_cmd(0xB0, 0x00); // reset row 0
        write_cmd(0x00, 0x10); // reset to column 0

        set_data_mode();
        dma_channel_configure(
            dma_channel,
            &dma_config,
            &spi_get_hw(spi)->dr,
            get_back_buffer().get_buffer().data(),
            get_back_buffer().get_buffer().size(),
            true);

        front_buffer_idx = (front_buffer_idx + 1) % frame_buffers.size();
    }

private:
    void select_device()
    {
        gpio_put(cs, 0);
    }

    void deselect_device()
    {
        gpio_put(cs, 1);
    }

    void set_cmd_mode()
    {
        gpio_put(dc, 0);
    }

    void set_data_mode()
    {
        gpio_put(dc, 1);
    }

    void write_cmd(uint8_t cmd)
    {
        spi_write_blocking(spi, &cmd, 1);
    }

    void write_cmd(uint8_t cmd, uint8_t value)
    {
        write_cmd(cmd);
        write_cmd(value);
    }

    void write_data(const uint8_t *data, size_t len)
    {
        spi_write_blocking(spi, data, len);
        return;
    }
};

template <typename>
constexpr bool is_sh1122_driver_v = false;

template <
    size_t width_,
    size_t height_,
    uint8_t sck_,
    uint8_t mosi_,
    uint8_t dc_,
    uint8_t cs_,
    uint8_t rst_>
constexpr bool is_sh1122_driver_v<SH1122Driver<width_, height_, sck_, mosi_, dc_, cs_, rst_>> = true;

template <typename sh1122_driver_>
concept is_sh1122_driver = is_sh1122_driver_v<std::remove_cvref_t<sh1122_driver_>>;
