////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_MESSAGEBUFFER_H
#define __APP_MESSAGEBUFFER_H


#include <NsCore/String.h>

#include "JsonObject.h"


namespace Noesis
{
struct ServerState;

class MessageBuffer final
{
public:
    MessageBuffer();
    ~MessageBuffer() = default;
    void HandleChar(char c);
    JsonObject GetBody() const;
    const char* GetBufferStr() const;
    bool IsMessageCompleted() const;
    void Clear();

private:
    String mBuffer;
    uint32_t mContentLength;
    uint32_t mCurrentState;
};
}

#endif