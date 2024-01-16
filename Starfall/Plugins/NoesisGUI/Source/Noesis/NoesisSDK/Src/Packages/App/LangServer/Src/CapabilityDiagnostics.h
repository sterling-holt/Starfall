////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_CAPABILITYDIAGNOSTICS_H
#define __APP_CAPABILITYDIAGNOSTICS_H


#include <NsApp/LangServer.h>


namespace Noesis
{
class Workspace;
class BaseString;
struct DocumentContainer;
struct TextPosition;

class CapabilityDiagnostics final
{
public:
    static void GetDiagnosticsMessage(RenderCallback renderCallback, 
		DocumentContainer& document, Workspace& workspace, int renderWidth, int renderHeight,
		double renderTime, BaseString& messageBuffer);
private:
    CapabilityDiagnostics() = default;
};
}

#endif