
#pragma once

#include "NXGeom.hpp"

struct NXBlit
{

static void
blit (
     NXBitmap * src,
     NXRect   * src_clip, 

     NXBitmap * dst,
     NXRect   * dst_clip,
     NXCanvasState * state
     ) 
{
    if (src_clip->size != dst_clip->size)
        panic();                    // We do not support stretch blit right now


    NXPoint src_pt = src_clip->origin;
    NXPoint dst_pt = dst_clip->origin;

    for (int row = 0; row < src_clip->size.h; row++) 
    {
        for (int col = 0; col < src_clip->size.w; col++) 
        {
            NXColor pixel = src->get_pixel(src_pt);

            if (pixel.is_valid())
            {
                if (state->rop == NXCanvasROP::COPY)
                {
                    if (state->mono_color_txform)
                        pixel = pixel.is_black() ? state->bg : state->fg;
                    dst->set_pixel(dst_pt, pixel);
                }
                else
                if (state->rop == NXCanvasROP::SPRITE)
                {
                    if ( ! pixel.is_black() )
                    {
                        if (state->mono_color_txform)
                            dst->set_pixel(dst_pt, state->fg);
                        else
                            dst->set_pixel(dst_pt, pixel);
                    }
                }
                else
                if (state->rop == NXCanvasROP::XOR)
                {
                    NXColor dst_pixel = dst->get_pixel(dst_pt);
                    if (dst_pixel.is_valid())
                    {
                        NXColor color = pixel.exclusive_or(dst_pixel);
                        dst->set_pixel(dst_pt, color);
                    }
                }
                else
                    panic();
            }

            src_pt.x++;
            dst_pt.x++;
        }

        src_pt.x = src_clip->origin.x;
        src_pt.y ++;

        dst_pt.x = dst_clip->origin.x;
        dst_pt.y ++;
    }
}

static void
fill_rect (
     NXBitmap * dst_bmp,
     NXRect   * dst_rect,
     NXColor    color,
     bool       xorop = false
     ) 
{
    // Validate rect

    NXPoint dst_pt = dst_rect->origin;

    for (int row = 0; row < dst_rect->size.h; row++) 
    {
        for (int col = 0; col < dst_rect->size.w; col++) 
        {
            if (xorop)
            {
                NXColor dst_pixel = dst_bmp->get_pixel(dst_pt);
                if (dst_pixel.is_valid())
                {
                    NXColor pixel = color.exclusive_or(dst_pixel);
                    dst_bmp->set_pixel(dst_pt, pixel);
                }
            }
            else
                dst_bmp->set_pixel(dst_pt, color);


            dst_pt.x++;
        }

        dst_pt.x = dst_rect->origin.x;
        dst_pt.y ++;
    }
}
};
