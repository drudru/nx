
#pragma once

#include "common.h"

// A convenience class that extracts
// Font Resource data from ancient 
// resources embedded in old EXE files
//
#include "SystemFormats/NXMSDOSHeader.hpp"
#include "SystemFormats/NXMSEXEHeader.hpp"

struct NXFontFON
{
    NXMmapFile * data;
    NXMemStream  stream;

    NXBitmap     bitmap;
    NXRect       font_bbx;
    NXRect       char_box;
    char       * current;
    U32          font_rsc_pos;
    FONTRES    * fon; 

    void init()
    {
        if (!data)
            panic();

        NXMSDOSHeader dos_header;
        dos_header.data = data;
        dos_header.init();
        if (!dos_header.valid)
            panic();

        NXMSEXEHeader exe_header;
        exe_header.data = data;
        exe_header.start = dos_header.lfanew;
        exe_header.init();

        font_rsc_pos = exe_header.locate_resource(RT_FONT, 0x8004);
        printf("font_rsc_pos = %0x\n", font_rsc_pos);

        stream.data = data;
        stream.little_endian = true;
        stream.init();

        {
            fon = (FONTRES *) (stream.data->ptr() + font_rsc_pos);

            printf("  FONT   \n");
            printf("  ----   \n");
            printf("    version = %0x\n", fon->dfVersion);
            printf("    size    = %0x\n", fon->dfSize);
            printf("    type    = %0x\n", fon->dfType);
            printf("    copy    = %s\n",  fon->dfCopyright);
            printf("    name    = %s\n",  stream.data->ptr() + fon->dfFace + font_rsc_pos);
            printf("    charset = %d\n",  fon->dfCharSet);
            printf("    width   = %d\n",  fon->dfPixWidth);
            printf("    height  = %d\n",  fon->dfPixHeight);
            printf("    first   = %d\n",  fon->dfFirstChar);
            printf("    last    = %d\n",  fon->dfLastChar);
            printf("  ----    \n");

            char_box = {{0,0}, {0, 0}};
            char_box.size.w = fon->dfPixWidth;
            char_box.size.h = fon->dfPixHeight;
        }

        void * fbp = calloc(1, char_box.size.w * char_box.size.h);
        bitmap.data = (U8 *)fbp;
        bitmap.rect = char_box;
        bitmap.chans = NXColorChan::GREY1;
    }

    bool render_glyph(int c)
    {
        // Clear our Bitmap
        NXBlit::fill_rect(&bitmap, &bitmap.rect, NXColor{   0,   0,   0, 255 });

        // Locate character in table
        if ((c < fon->dfFirstChar) || (c > fon->dfLastChar))
            return false;

        printf("     glyph = %0x\n", c);

        U16 char_width  = fon->dfCharTable[(c - fon->dfFirstChar) << 1];
        U16 char_offset = fon->dfCharTable[((c - fon->dfFirstChar) << 1) + 1];
        printf("     width = %0x\n", char_width);
        printf("    offset = %0x\n", char_offset);

        // Now Get Bitmap and Render
        NXColor fg{ 255, 255, 255, 255 };
        NXPoint pt{ 0, 0 };
        {
            for (int row = 0; row < char_box.size.h ; row++)
            {
                U8 bits = *( ((U8 *)fon) + char_offset + row );
                pt.y = row;
                pt.x = 0;

                for (int col = 0; col < char_box.size.w; col++)
                {
                    if (bits & 0x80)
                        bitmap.set_pixel(pt, fg);
                    bits <<= 1;
                    pt.x  += 1;
                }
            }
        }


        for (int row = 0; row < char_box.size.h; row++)
        {
          for (int col = 0; col < char_box.size.w; col++)
              write(1,
                      bitmap.data[ (row * char_box.size.w) + col] ?
                      "*" : ".", 1);
          write(1, "\n", 1);
        }

        return true;
    }

};

