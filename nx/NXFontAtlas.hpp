
#pragma once

#include "common.h"
#include "NXGeom.hpp"
#include "NXBlit.hpp"

struct NXFontAtlas
{
    NXBitmap  * atlas;
    NXRect      rect;               // Subset of bitmap
    NXSize      size;               // Size in cols, rows
    NXSize      char_size;          // Char size

    void init()
    {
        I16 tmp_w = rect.size.w / size.w;
        I16 tmp_h = rect.size.h / size.h;
        char_size = { tmp_w, tmp_h };
    }

    NXRect get_glyph_rect(const char c)
    {
        I16 col = c % size.w;
        I16 row = c / size.w;

        NXRect src_rect =
        {
            {
                (I16)((col * char_size.w) + rect.origin.x),
                (I16)((row * char_size.h) + rect.origin.y)
            },
            char_size
        };

        return src_rect;
    }

    /*
    void draw_rect(NXBitmap * dest, NXRect rect)
    {
        NXPoint pt;

        char str[] = "X";

        str[0] = 205;
        for (U8 col = 1; col < (rect.size.w - 1); col++)
        {
            pt.x = rect.origin.x + (col * char_size.w);

            pt.y = rect.origin.y;
            render(dest, pt, str);

            pt.y = rect.origin.y + ((rect.size.h - 1) * char_size.h);
            render(dest, pt, str);
        }

        str[0] = 186;
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
    }
    */
};

