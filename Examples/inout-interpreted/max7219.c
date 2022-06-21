// for code insight to work properly
#include "../../yacl/yacl.h"

//  Example how to conveniently write to 4 cascaded MAX7219s
//
//  spi write <displays> <registers> <data>
//  select display -- 0xff sends the <data> to all cascaded <displays>
//                 -- 0 sends <data> as unique bytes to the same <registers> to all <displays>

// reverse byte to match hex values to expected orientation of display
uint8_t revr_byte(uint8_t b);

void usr_spi_write(yacl_inout_data_t *inout_data)
{
    uint32_t data32 = inout_data->data;
    uint16_t data16_out[4];

    // one address. unique data. all 4 displays
    if (inout_data->addr == 0)
    {
        for (int32_t i = 3; i >= 0; --i)
            data16_out[i] = ((inout_data->beg_reg & 0xff) << 8) | revr_byte(data32 >> (8 * i) & 0xff);

    }
    // one address. one data. selected displays
    else
    {
        for (uint32_t i = 0; i < 4; ++i)
        {
            if (inout_data->addr & (0x01 << i))
                data16_out[i] = ((inout_data->beg_reg & 0xff) << 8) | data32;
            else
                data16_out[i] = 0x0000;
        }
    }

    // multiple registers
    while (1)
    {
        // call spi transmit code
        // expected to pulse CS as high -> low -> high
        // spi_helper(data16_out);

        if (!(--inout_data->range))
            return;

        if (inout_data->beg_reg > inout_data->end_reg)
        {
            for (uint32_t i = 0; i < 4; ++i)
                data16_out[i] -= 0x100;
        }
        else if (inout_data->beg_reg < inout_data->end_reg)
        {
            for (uint32_t i = 0; i < 4; ++i)
                data16_out[i] += 0x100;
        }
    }
}
