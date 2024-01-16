////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_MESSAGEREADER_H
#define __APP_MESSAGEREADER_H


#include <NsCore/StringFwd.h>
#include <NsApp/LangServer.h>


namespace Noesis
{
class JsonObject;
class Workspace;
struct ServerState;

class MessageReader final
{
public:
    static void HandleMessage(RenderCallback renderCallback, const JsonObject& body,
        Workspace& workspace, double currentTime, BaseString& responseBuffer);
private:
    MessageReader() = default;
};
}

#endif