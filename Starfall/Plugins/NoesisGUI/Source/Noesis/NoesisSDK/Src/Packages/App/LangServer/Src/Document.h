////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_DOCUMENT_H
#define __APP_DOCUMENT_H


#include <NsCore/String.h>
#include <NsCore/Vector.h>
#include <NsGui/Uri.h>


namespace Noesis
{

struct DocumentContainer
{
    Uri uri;
    FixedString<256> vsCodeUri;
    bool isDirty;
    double lastUpdateTime;
    FixedString<4096> text;
    Vector<uint32_t, 256> lineStartPositions;
    uint32_t version;
    bool hasValidTree;
};

struct TextPosition
{
    uint32_t line;
    uint32_t character;
    uint32_t textPosition;
};

struct TextRange
{
    TextPosition start;
    TextPosition end;
};
}

#endif
