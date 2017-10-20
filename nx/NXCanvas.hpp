
#pragma once

#include "NXGeom.hpp"

struct NXColor
{
    uint8_t     r;
    uint8_t     g;
    uint8_t     b;
    uint8_t     a;

    bool is_black()
    {
        return (true
                && (r == 0)
                && (g == 0)
                && (b == 0));
    }

    void exclusive_or(NXColor c)
    {
        r = r ^ c.r;
        g = g ^ c.g;
        b = b ^ c.b;
    }
};

enum NXColorChan : uint8_t
{
    GREY1  = 1,
    RGB565 = 2
}; 

struct NXBitmap
{
    uint8_t   * data;
    NXRect      rect;
    NXColorChan chans;

    NXColor get_pixel(NXPoint pt)
    {
        validate_point(pt);

        NXColor c = { 0, 0, 0, 255 };

        if (chans == NXColorChan::GREY1)
        {
            uint8_t * p = data + (rect.size.w * pt.y * 1) + (pt.x * 1);

            uint8_t byte = (uint8_t)(*p);

            c.r = byte;
            c.g = byte;
            c.b = byte;
        }
        else
        if (chans == NXColorChan::RGB565)
        {
            uint8_t * p = data + (rect.size.w * pt.y * 2) + (pt.x * 2);

            uint16_t word = (uint16_t)(*p);

            c.r = (word >> 11) & 0x1f;
            c.g = (word >>  5) & 0x3f;
            c.b = (word >>  0) & 0x1f;
        }

        return c;
    }

    void set_pixel(NXPoint pt, NXColor c)
    {
        validate_point(pt);

        if (chans == NXColorChan::GREY1)
        {
            uint8_t byte = (c.r + c.g + c.b) / 3;

            uint8_t * p = data + (rect.size.w * pt.y * 1) + (pt.x * 1);
            *((uint8_t *)p) = byte;
        }
        else
        if (chans == NXColorChan::RGB565)
        {
            uint16_t word = 0;

            word |= (c.r >> 3) << 11;
            word |= (c.g >> 2) <<  5;
            word |= (c.b >> 3) <<  0;

            uint8_t * p = data + (rect.size.w * pt.y * 2) + (pt.x * 2);
            *((uint16_t *)p) = word;
        }

    }

    void validate_point(NXPoint pt)
    {
        if (!rect.is_point_in(pt))
            panic();
    }
};

enum class NXCanvasROP : uint8_t
{
    COPY,       // Just copy src
    SPRITE,     // Only write non-zero bits
    XOR         // XOR the src and dest
}; 

struct NXCanvasState
{
    NXColor fg  = { 255, 255, 255, 255 };
    NXColor bg  = {   0,   0,   0, 255 };

    bool mono_color_txform = false;
    NXCanvasROP rop = NXCanvasROP::COPY;        // Raster Op
};


// C++ BULLSHIT
#include "NXBlit.hpp"
#include "NXFontAtlas.hpp"

struct NXCanvas
{
    NXBitmap        bitmap;
    NXCanvasState   state;

    NXColor get_pixel(NXPoint pt)
    {
        return bitmap.get_pixel(pt);
    }

    void set_pixel(NXPoint pt, NXColor c)
    {
        bitmap.set_pixel(pt, c);
    }

    // Clears canvas to bg color
    void clear()
    {
        fill_rect(&bitmap.rect, state.bg);
    }

    void fill_rect(NXRect * dst_rect, NXColor color) 
    {
        NXBlit::fill_rect(&bitmap, dst_rect, color);
    }

    NXRect font_rect_convert(NXFontAtlas * font, NXRect txt_grid)
    {
        // Convert txt grid to pixel / gfx grid
        NXRect  gfx_rect;
        NXRect  glyph_rect = font->get_glyph_rect(196);  // '-' char

        gfx_rect.origin.x = txt_grid.origin.x * glyph_rect.size.w;
        gfx_rect.origin.y = txt_grid.origin.y * glyph_rect.size.h;

        gfx_rect.size.w = txt_grid.size.w * glyph_rect.size.w;
        gfx_rect.size.h = txt_grid.size.h * glyph_rect.size.h;

        return gfx_rect;
    }


    void draw_font(NXFontAtlas * font, NXPoint start_pt, const char * txt)
    {
        NXRect  dst_rect = { start_pt, { 0, 0 } };

        while (*txt)
        {
            char c = *txt;

            NXRect src_rect = font->get_glyph_rect(c);

            dst_rect.size = src_rect.size;

            NXBlit::blit(font->atlas, &src_rect,
                         &bitmap,     &dst_rect,
                         &state);

            txt  += 1;
            dst_rect.origin.x += src_rect.size.w;
        }
    }

    /**
     * draw_font_rect
     * draws a rect in box chars. Works best with monspace font
     * TODO: txt_rect is in text grid coords!
     */
    void draw_font_rect(NXFontAtlas * font, NXRect txt_rect)
    {
        //font->draw_rect(&bitmap, rect);
        //
        NXRect  src_rect;
        NXRect  dst_rect;

        // Top Border
        // src_rect = font->get_glyph_rect(205);  // '=' char
        src_rect = font->get_glyph_rect(196);  // '-' char
        dst_rect = { { 0, 0 }, src_rect.size };

        dst_rect.origin.y = txt_rect.origin.y * src_rect.size.h;

        for (U8 col = 1; col < (txt_rect.size.w - 1); col++)
        {
            dst_rect.origin.x = (txt_rect.origin.x * src_rect.size.w) +
                                (col * src_rect.size.w);

            NXBlit::blit(font->atlas, &src_rect,
                         &bitmap,     &dst_rect,
                         &state);
        }

        // Bottom Border
        src_rect = font->get_glyph_rect(196);  // '-' char
        dst_rect = { { 0, 0 }, src_rect.size };

        dst_rect.origin.y = (txt_rect.origin.y * src_rect.size.h) +
                            ((txt_rect.size.h - 1) * src_rect.size.h);

        for (U8 col = 1; col < (txt_rect.size.w - 1); col++)
        {
            dst_rect.origin.x = (txt_rect.origin.x * src_rect.size.w) +
                                (col * src_rect.size.w);

            NXBlit::blit(font->atlas, &src_rect,
                         &bitmap,     &dst_rect,
                         &state);
        }

        // Left Border
        src_rect = font->get_glyph_rect(179);  // '|' char
        dst_rect = { { 0, 0 }, src_rect.size };

        dst_rect.origin.x = txt_rect.origin.x * src_rect.size.w;

        for (U8 row = 1; row < (txt_rect.size.h - 1); row++)
        {
            dst_rect.origin.y = (txt_rect.origin.y * src_rect.size.h) +
                                (row * src_rect.size.h);

            NXBlit::blit(font->atlas, &src_rect,
                         &bitmap,     &dst_rect,
                         &state);
        }

        // Right Border
        src_rect = font->get_glyph_rect(179);  // '|' char
        dst_rect = { { 0, 0 }, src_rect.size };

        dst_rect.origin.x = (txt_rect.origin.x * src_rect.size.w) +
                            ((txt_rect.size.w - 1) * src_rect.size.w);

        for (U8 row = 1; row < (txt_rect.size.h - 1); row++)
        {
            dst_rect.origin.y = (txt_rect.origin.y * src_rect.size.h) +
                                (row * src_rect.size.h);

            NXBlit::blit(font->atlas, &src_rect,
                         &bitmap,     &dst_rect,
                         &state);
        }

        // Top Left Corner
        src_rect = font->get_glyph_rect(218);  // ',-' char
        dst_rect = { { 0, 0 }, src_rect.size };
        {
            dst_rect.origin.x = (txt_rect.origin.x * src_rect.size.w);
            dst_rect.origin.y = (txt_rect.origin.y * src_rect.size.h);

            NXBlit::blit(font->atlas, &src_rect,
                         &bitmap,     &dst_rect,
                         &state);
        }

        // Top Right Corner
        src_rect = font->get_glyph_rect(191);  // '-,' char
        dst_rect = { { 0, 0 }, src_rect.size };
        {
            dst_rect.origin.x = (txt_rect.origin.x * src_rect.size.w) +
                                ((txt_rect.size.w - 1) * src_rect.size.w);

            dst_rect.origin.y = (txt_rect.origin.y * src_rect.size.h);

            NXBlit::blit(font->atlas, &src_rect,
                         &bitmap,     &dst_rect,
                         &state);
        }


        // Bottom Left Corner
        src_rect = font->get_glyph_rect(192);  // '`-' char
        dst_rect = { { 0, 0 }, src_rect.size };
        {
            dst_rect.origin.x = (txt_rect.origin.x * src_rect.size.w);
            dst_rect.origin.y = (txt_rect.origin.y * src_rect.size.h) +
                                ((txt_rect.size.h - 1) * src_rect.size.h);

            NXBlit::blit(font->atlas, &src_rect,
                         &bitmap,     &dst_rect,
                         &state);
        }

        // Bottom Right Corner
        src_rect = font->get_glyph_rect(217);  // '-`' char
        dst_rect = { { 0, 0 }, src_rect.size };
        {
            dst_rect.origin.x = (txt_rect.origin.x * src_rect.size.w) +
                                ((txt_rect.size.w - 1) * src_rect.size.w);

            dst_rect.origin.y = (txt_rect.origin.y * src_rect.size.h) +
                                ((txt_rect.size.h - 1) * src_rect.size.h);

            NXBlit::blit(font->atlas, &src_rect,
                         &bitmap,     &dst_rect,
                         &state);
        }

        /*
        src_rect = font->get_glyph_rect(186);  // '||' char
        for (U8 row = 1; row < (rect.size.h - 1); row++)
        {
            pt.y = rect.origin.y + (row * char_size.h);

            pt.x = rect.origin.x;
            render(dest, pt, str);

            pt.x = rect.origin.x + ((rect.size.w - 1) * char_size.w);
            render(dest, pt, str);
        }

        pt = rect.origin;
        str[0] = 201;
        render(dest, pt, str);

        pt.x += (rect.size.w - 1) * char_size.w;
        str[0] = 187;
        render(dest, pt, str);

        pt = rect.origin;
        pt.y += (rect.size.h - 1) * char_size.h;
        str[0] = 200;
        render(dest, pt, str);

        pt.x += (rect.size.w - 1) * char_size.w;
        str[0] = 188;
        render(dest, pt, str);
        */
    }
};

