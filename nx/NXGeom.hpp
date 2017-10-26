
#pragma once

#include "common.h"

#define NXCoord int16_t

struct NXPoint
{
    NXCoord x;
    NXCoord y;
};

struct NXSize
{
    NXCoord w;
    NXCoord h;

    bool operator==(const NXSize& rhs) const
    {
        return (w == rhs.w) && (h == rhs.h);
    }

    bool operator!=(const NXSize& rhs) const
    {
        return !operator==(rhs);
    }

};

struct NXRect
{
    NXPoint     origin;
    NXSize      size;

    bool is_point_in(NXPoint pt)
    {
        if (pt.x < origin.x)
            return false;
        if (pt.y < origin.y)
            return false;
        if (pt.x >= (origin.x + size.w))
            return false;
        if (pt.y >= (origin.y + size.h))
            return false;

        return true;
    }

    NXRect center_in(NXRect outer)
    {
        NXRect new_rect = *this;
        new_rect.origin = outer.origin;

        // Todo - validate size maybe?

        new_rect.origin.x += (outer.size.w - new_rect.size.w) / 2;
        new_rect.origin.y += (outer.size.h - new_rect.size.h) / 2;

        return new_rect;
    }

    // Keeps the rect center
    NXRect inset(int delta)
    {
        NXRect new_rect = *this;

        new_rect.origin.x += delta;
        new_rect.origin.y += delta;

        new_rect.size.w -= delta * 2;
        new_rect.size.h -= delta * 2;

        return new_rect;
    }
};

