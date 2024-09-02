#pragma once

#include <Windows.h>
#include "Renderer/Bitmap.h"
#include "Renderer/MMBitmap.h"
#include "Util/mmath.h"



union COLOR
{
    struct { BYTE B, G, R, A; };
    BYTE    color[4];
    DWORD   dwColorCode;
};

struct BITMAP_HANDLE
{
    Bitmap*     pBitmap;
    MMBitmap*   pCompressedBitmap;
    DWORD       dwRefCount;
    void*       pSearchHandle;
    LINK_NODE	linkNode;
};

struct FONT_HANDLE
{

};
