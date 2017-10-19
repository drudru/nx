
#pragma once

#include "common.h"
//#include "NXGeom.hpp"
//#include "NXBlit.hpp"
#include "NXStrTokenizer.hpp"

// A convenience class that wraps 
// an Adobe Glyph Bitmap Distribution Format 
// (aka bitmap font file)
//
// This is meant to be used by an offline 
// process

// Insert ASCII DIAGRAM HERE of Font Cell

struct NXFontBDF
{
    NXMmapFile * data;
    NXBitmap     bitmap;
    NXRect       font_bbx;
    NXRect       char_box;
    char       * current;

    void init()
    {
        if (!data)
            panic();
        current = data->ptr();

        get_bounding_box();
        char_box = NXRect{{0, 0}, {0, 0}};
        char_box.size = font_bbx.size;

        void * fbp = calloc(1, char_box.size.w * char_box.size.h);
        bitmap.data = (U8 *)fbp;
        bitmap.rect = char_box;
        bitmap.chans = NXColorChan::GREY1;
    }

    // Uses a heuristic that, we will do best by searching
    // From the last point we found a token
    char * locate_token(const char * needle)
    {
        char * result_p = strstr(current, needle); 
        if (result_p)
        {
            current = result_p;
            return result_p;
        }

        current = data->ptr();
        result_p = strstr(current, needle); 
        if (result_p)
            current = result_p;

        return result_p;
    }

    char * locate_encoding(char * num_str)
    {
        if (strlen(num_str) > 10)
            panic();

        char buff[] = "ENCODING 1234567890";
        strcpy(buff + 9, num_str);

        return locate_token(buff);
    }
    void get_bounding_box()
    {
        // Locate FONTBOUNDINGBOX line
        char * p = locate_token("FONTBOUNDINGBOX"); 
        if (!p) panic();

        // Split by space
        NXStringTokenizer tokens(p);

        auto token = tokens.get_next(); // FONTBOUNDINGBOX
        token = tokens.get_next(); // WIDTH
        font_bbx.size.w = token.to_int();
        token = tokens.get_next(); // HEIGHT
        font_bbx.size.h = token.to_int();

        // Negative values here push the origin 'up and to the right'
        // in the bounding box
        token = tokens.get_next(); // Xoff
        font_bbx.origin.x = token.to_int();
        token = tokens.get_next(); // Yoff
        font_bbx.origin.y = token.to_int();

        if (font_bbx.size.w > 32)
            panic();    // We cannot handle > 32 bit bitmaps at this time
    }

    bool render_glyph(int c)
    {
        // Clear our Bitmap
        NXBlit::fill_rect(&bitmap, &bitmap.rect, NXColor{   0,   0,   0, 255 });

        char glyph_num_str[NXNumBase::MAX_U32_STR_B10 + 1] = {0};

        NXNumBase::get_ascii_from_int(c, 10, glyph_num_str);

        NXPStr glyph_debug = glyph_num_str;
        glyph_debug.write(1);
        write(1, "\n", 1);

        char * p = locate_encoding(glyph_num_str);
        if (!p)
            return false;

        // Parse Bounding Box
        NXRect bbx;
        {
            p = locate_token("BBX");
            if (!p) panic();

            // Split by space
            NXStringTokenizer tokens(p);

            // BBX is from origin of bottom left
            auto token = tokens.get_next(); // BBX
            token = tokens.get_next(); // WIDTH
            bbx.size.w = token.to_int();
            token = tokens.get_next(); // HEIGHT
            bbx.size.h = token.to_int();
            token = tokens.get_next(); // offset_x
            bbx.origin.x = token.to_int();
            token = tokens.get_next(); // offset_y
            bbx.origin.y = token.to_int();
        }

        // Now Get Bitmap and Render
        NXColor fg{ 255, 255, 255, 255 };
        {
            p = locate_token("BITMAP");
            if (!p) panic();

            NXStringTokenizer tokens(p);
            auto token = tokens.get_next(); // BITMAP

            for (int row = 0; row < bbx.size.h ; row++)
            {
                token = tokens.get_next(); // actual bitmap
                U32 bdf_bits = token.to_int(16);
                U32 byte_len = token._len;

                // Adobe BDF origin is bottom-left,
                // YET, they present bitmap top to bottom!
                // Our NX system uses origin at top-left

                // I worked this out on paper
                // y = font_bbx.height - 
                //       ((char_height - font_bbx.y_off) + 
                //         char_yoff)) + row

                NXPoint pt;
                pt.y = font_bbx.size.h - 
                       ( (bbx.size.h - font_bbx.origin.y) + bbx.origin.y)
                       + row;

                // Scan bitmap from hi-order (left)
                // Due to the format, it is easier to reverse the bitmap
                // Given the number of bytes read-in
                U32 bits = 0;
                U8  bit_pos = byte_len * 4 - 1;
                U8  new_bit_pos = 0;
                for (int col = 0; col < bbx.size.w; col++)
                {
                    if (bdf_bits & (1 << bit_pos))
                        bits |= (1 << new_bit_pos);
                    bit_pos--;
                    new_bit_pos++;
                }


                for (int col = 0; col < bbx.size.w; col++)
                {
                    pt.x = (- font_bbx.origin.x) + bbx.origin.x + col;
                    if (bits & 1)
                        bitmap.set_pixel(pt, fg);
                    bits >>= 1;
                }
            }
            token = tokens.get_next(); // ENDCHAR
            if (token != "ENDCHAR")
                panic();
        }


        for (int row = 0; row < font_bbx.size.h; row++)
        {
          for (int col = 0; col < font_bbx.size.w; col++)
              write(1,
                      bitmap.data[ (row * font_bbx.size.w) + col] ?
                      "*" : ".", 1);
          write(1, "\n", 1);
        }

        return true;
    }

    /*
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
    */

};

