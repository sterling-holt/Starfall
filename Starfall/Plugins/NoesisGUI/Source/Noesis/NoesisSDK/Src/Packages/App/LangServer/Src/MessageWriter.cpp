////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsCore/String.h>
#include "MessageWriter.h"

#include "JsonBuilder.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::MessageWriter::CompleteMessage(JsonBuilder& body, BaseString& messageBuffer)
{
    messageBuffer.AppendFormat("Content-Length: %u\r\n", body.Size());
    messageBuffer.Append("Content-Type: application/vscode-jsonrpc;charset=utf-8\r\n");
    messageBuffer.Append("\r\n");
    messageBuffer.Append(body.Str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::MessageWriter::CreateResponse(uint32_t id, const char* result, BaseString& messageBuffer)
{
    JsonBuilder body;
    body.StartObject();
    body.WritePropertyName("id");
    body.WriteValue(id);
    body.WritePropertyName("jsonrpc");
    body.WriteValue("2.0");
    body.WritePropertyName("result");
    body.WriteRawValue(result);
    body.EndObject();

    CompleteMessage(body, messageBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::MessageWriter::CreateErrorResponse(int id, int errorCode, const char* errorMessage,
    BaseString& messageBuffer)
{
    JsonBuilder body;
    body.StartObject();
    body.WritePropertyName("id");
    body.WriteValue(id);
    body.WritePropertyName("jsonrpc");
    body.WriteValue("2.0");
    body.WritePropertyName("error");
    body.StartObject();
    body.WritePropertyName("code");
    body.WriteValue(errorCode);
    body.WritePropertyName("message");
    body.WriteValue(errorMessage);
    body.EndObject();
    body.EndObject();

    CompleteMessage(body, messageBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::MessageWriter::CreateNotification(const char* method, const char* params, 
    BaseString& messageBuffer)
{
    JsonBuilder body;
    body.StartObject();
    body.WritePropertyName("jsonrpc");
    body.WriteValue("2.0");
    body.WritePropertyName("method");
    body.WriteValue(method);
    body.WritePropertyName("params");
    body.WriteRawValue(params);
    body.EndObject();

    CompleteMessage(body, messageBuffer);
}



