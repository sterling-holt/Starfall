////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_MESSAGEWRITER_H
#define __APP_MESSAGEWRITER_H


#include <NsCore/String.h>

#include "JsonBuilder.h"


namespace Noesis
{
class MessageWriter final
{
public:
    static void CompleteMessage(JsonBuilder& body, BaseString& messageBuffer);
    static void CreateResponse(uint32_t id, const char* result, BaseString& messageBuffer);
    static void CreateErrorResponse(int id, int errorCode, const char* errorMessage,
        BaseString& messageBuffer);
    static void CreateNotification(const char* method, const char* params, 
        BaseString& messageBuffer);
private:
    MessageWriter() = default;
};
}

#endif