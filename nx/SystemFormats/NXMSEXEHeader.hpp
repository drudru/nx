
#pragma once

#include "common.h"
#include "NXMemStream.hpp"

#include <stdio.h>


#pragma pack(push, 1)
typedef struct _NAMEINFO {
    U16 rnOffset;
    U16 rnLength;
    U16 rnFlags;
    U16 rnID;
    U16 rnHandle;
    U16 rnUsage;
} NAMEINFO;

typedef struct _TYPEINFO {
    U16         rtTypeID;
    U16         rtResourceCount;
    U32         rtReserved;
    NAMEINFO    rtNameInfo[];
} TYPEINFO;

struct RESTABLE
{
    U16 rscAlignShift;
    TYPEINFO rscTypes[];
};

typedef struct _FontDirEntry
{ 
    U16    dfOrdinal; 
    U16    dfVersion; 
    U32    dfSize; 
    char   dfCopyright[60]; 
    U16    dfType; 
    U16    dfPoints; 
    U16    dfVertRes; 
    U16    dfHorizRes; 
    U16    dfAscent; 
    U16    dfInternalLeading; 
    U16    dfExternalLeading; 
    U8     dfItalic; 
    U8     dfUnderline; 
    U8     dfStrikeOut; 
    U16    dfWeight; 
    U8     dfCharSet; 
    U16    dfPixWidth; 
    U16    dfPixHeight; 
    U8     dfPitchAndFamily; 
    U16    dfAvgWidth; 
    U16    dfMaxWidth; 
    U8     dfFirstChar; 
    U8     dfLastChar; 
    U8     dfDefaultChar; 
    U8     dfBreakChar; 
    U16    dfWidthBytes; 
    U32    dfDevice; 
    U32    dfFace; 
    U32    dfReserved; 
    char   szDeviceName[1]; 
    char   szFaceName[]; 
} FontDirEntry;

typedef struct
{
    U16    dfVersion; 
    U32    dfSize; 
    char   dfCopyright[60]; 
    U16    dfType; 
    U16    dfPoints; 
    U16    dfVertRes; 
    U16    dfHorizRes; 
    U16    dfAscent; 
    U16    dfInternalLeading; 
    U16    dfExternalLeading; 
    U8     dfItalic; 
    U8     dfUnderline; 
    U8     dfStrikeOut; 
    U16    dfWeight; 
    U8     dfCharSet; 
    U16    dfPixWidth; 
    U16    dfPixHeight; 
    U8     dfPitchAndFamily; 
    U16    dfAvgWidth; 
    U16    dfMaxWidth; 
    U8     dfFirstChar; 
    U8     dfLastChar; 
    U8     dfDefaultChar; 
    U8     dfBreakChar; 
    U16    dfWidthBytes; 
    U32    dfDevice; 
    U32    dfFace; 
    U32    dfBitsPointer; 
    U32    dfBitsOffset; 
    U8     dfReserved; 
    /* Windows 3.x fonts
    U32    dfFlags; 
    U16    dfAspace; 
    U16    dfBspace; 
    U16    dfCspace; 
    U32    dfColorPointer; 
    U8     dfReserved1[16]; 
    */
    U16    dfCharTable[];  // (dfLastChar - dfFirstChar + 2) 
} FONTRES;
#pragma pack(pop)

enum WinResourceType
{
    RT_CURSOR=1,
    RT_BITMAP = 2,
    RT_ICON = 3,
    RT_MENU = 4,
    RT_DIALOG = 5,
    RT_STRING = 6,
    RT_FONTDIR = 7,
    RT_FONT = 8,
    RT_ACCELERATOR = 9,
    RT_RCDATA = 10,
    RT_MESSAGETABLE = 11,
    RT_GROUPCURSOR=12, RT_GROUPICON=14, RT_VERSION=16,
    RT_DLGINCLUDE=17, RT_PLUGPLAY=19, RT_VXD, RT_ANICURSOR, RT_ANIICON,
    RT_HTML, RT_MANIFEST
};


struct NXMSEXEHeader
{
    NXMmapFile * data   = nullptr;
    U16          start  = 0;
    U16          ne_rsrctab = 0;
    U16          alignment  = 0;

    NXMSEXEHeader()
    {
    }

    ~NXMSEXEHeader()
    {
    }

    void init()
    {
        NXMemStream stream;
        stream.data = data;
        stream.little_endian = true;
        stream.init();

        printf("start = %0x\n", start);

        stream.skip(start + 0x24);

        ne_rsrctab = stream.readU16();
        printf("ne_rsrctab = %0x\n", ne_rsrctab);

        stream.abs_pos(start + ne_rsrctab);
        RESTABLE * pResTable = (RESTABLE *) (stream.current);

        alignment = 1 << pResTable->rscAlignShift;   
        printf("rscAlignShift = %0d\n", pResTable->rscAlignShift);
        stream.skip(2);

        //TYPEINFO * pTypeInfo = (TYPEINFO *) stream.current;
        //TYPEINFO * pTypeInfo = pResTable->rscTypes;
        //printf("pResTable = %0x\n", pResTable);
        //printf("pTypeInfo = %0x\n", pTypeInfo);

        // search fir RT_FONT resource   
        //while ( pTypeInfo->rtTypeID != 0 )   
        U16 typeID = stream.readU16();
        while ( typeID != 0 )   
        {   
            printf("typeid = %0x ", typeID);
            if (typeID & 0x8000)
                identify_resource(typeID & 0x7fff);

            U16 res_count = stream.readU16();
            printf("   cnt = %0d\n", res_count);
            stream.skip(4);
            display_nameinfo(stream, res_count, typeID & 0x7fff);
            printf("\n");

            typeID = stream.readU16();
        }   
    }

    U32 locate_resource(U16 rscId, U16 nameId)
    {
        NXMemStream stream;
        stream.data = data;
        stream.little_endian = true;
        stream.init();

        stream.abs_pos(start + ne_rsrctab + 2);

        U16 typeID = stream.readU16();
        U32 result;
        while ( typeID != 0 )   
        {   
            bool found = false;
            if ((typeID & 0x7FFF) == rscId)
                found = true;

            U16 res_count = stream.readU16();
            printf("   cnt = %0d\n", res_count);
            stream.skip(4);
            result = get_nameinfo(stream, res_count, found, nameId);
            if (found)
                return result;

            typeID = stream.readU16();
        }   
        return 0;
    }

    void display_nameinfo(NXMemStream & stream, U16 res_count, U16 typeId)
    {
        NAMEINFO ni;
        for (int i = 0; i < res_count; i++)
        {
            U16 * p = (U16 *) &ni;
            for (int j = 0; j < 6; j++, p++)
                *p = stream.readU16();


            printf("  NAMEINFO\n");
            printf("  --------\n");
            printf("    offSet = %0x\n", ni.rnOffset);
            printf("    *aboff = %0x\n", ni.rnOffset * alignment);
            printf("    length = %0x\n", ni.rnLength);
            printf("    flags  = %0x\n", ni.rnFlags);
            printf("    id     = %0x\n", ni.rnID);
            printf("    handle = %0x\n", ni.rnHandle);
            printf("    usage  = %0x\n", ni.rnUsage);
            printf("  --------\n");

            switch (typeId)
            {
                case RT_FONTDIR:
                    display_fontdir(stream, ni.rnOffset * alignment);
                    break;
                case RT_FONT:
                    display_font(stream, ni.rnOffset * alignment);
                    break;
            }
        }

    }

    U32 get_nameinfo(NXMemStream & stream, U16 res_count, bool & found, U16 nameId)
    {
        found = false;
        NAMEINFO ni;
        for (int i = 0; i < res_count; i++)
        {
            U16 * p = (U16 *) &ni;
            for (int j = 0; j < 6; j++, p++)
                *p = stream.readU16();

            if (ni.rnID == nameId)
            {
                printf("FOUND!!\n");
                found = true;
                return ni.rnOffset * alignment;
            }
        }
        return 0;
    }


    void identify_resource(U16 type_id)
    {
        switch (type_id)
        {
            case RT_CURSOR:
                printf("RT_CURSOR");
                break;
            case RT_BITMAP:
                printf("RT_BITMAP");
                break;
            case RT_ICON:
                printf("RT_ICON");
                break;
            case RT_MENU:
                printf("RT_MENU");
                break;
            case RT_DIALOG:
                printf("RT_DIALOG");
                break;
            case RT_STRING:
                printf("RT_STRING");
                break;
            case RT_FONTDIR:
                printf("RT_FONTDIR");
                break;
            case RT_FONT:
                printf("RT_FONT");
                break;
            case RT_ACCELERATOR:
                printf("RT_ACCELERATOR");
                break;
            case RT_RCDATA:
                printf("RT_RCDATA");
                break;
            case RT_VERSION:
                printf("RT_VERSION");
                break;
            default:
                printf("UNKNOWN");
                break;
        }

        // TODO: add more enums

        printf("\n");
    }

    void display_fontdir(NXMemStream & stream, U32 position)
    {
        U16 fd_count = *((U16 *) (stream.data->ptr() + position));
        printf("   cnt = %0d\n", fd_count);
        FontDirEntry * fde; 
        fde = (FontDirEntry *) (stream.data->ptr() + position + 2);
        for (int i = 0; i < fd_count; i++)
        {

            printf("  FONTDIR\n");
            printf("  -------\n");
            printf("    ordinal = %d\n",  fde->dfOrdinal);
            printf("    version = %0x\n", fde->dfVersion);
            printf("    size    = %0x\n", fde->dfSize);
            printf("    type    = %0x\n", fde->dfType);
            printf("    copy    = %s\n",  fde->dfCopyright);
            printf("    name    = %s\n",  fde->szFaceName);
            printf("    width   = %d\n",  fde->dfPixWidth);
            printf("    height  = %d\n",  fde->dfPixHeight);
            printf("    first   = %d\n",  fde->dfFirstChar);
            printf("    last    = %d\n",  fde->dfLastChar);
            printf("  --------\n");

            // Next record is past szFaceName NULL
            U8 extra = strlen(fde->szFaceName) + 1;
            fde = (FontDirEntry *) (((U8 *)fde) + sizeof(*fde) + extra);
        }
    }

    void display_font(NXMemStream & stream, U32 position)
    {
        {
            FONTRES * fon; 
            fon = (FONTRES *) (stream.data->ptr() + position);

            printf("  FONT   \n");
            printf("  ----   \n");
            printf("    version = %0x\n", fon->dfVersion);
            printf("    size    = %0x\n", fon->dfSize);
            printf("    type    = %0x\n", fon->dfType);
            printf("    copy    = %s\n",  fon->dfCopyright);
            printf("    name    = %s\n",  stream.data->ptr() + fon->dfFace + position);
            printf("    charset = %d\n",  fon->dfCharSet);
            printf("    width   = %d\n",  fon->dfPixWidth);
            printf("    height  = %d\n",  fon->dfPixHeight);
            printf("    first   = %d\n",  fon->dfFirstChar);
            printf("    last    = %d\n",  fon->dfLastChar);
            printf("    default = %d\n",  fon->dfDefaultChar);
            printf("    break   = %d\n",  fon->dfBreakChar);
            printf("    wbytes  = %0x\n", fon->dfWidthBytes);
            printf("    bitsOff = %0x\n", fon->dfBitsOffset);
            printf("    faceOff = %0x\n", fon->dfFace);
            printf("  ----    \n");
        }
    }

};

