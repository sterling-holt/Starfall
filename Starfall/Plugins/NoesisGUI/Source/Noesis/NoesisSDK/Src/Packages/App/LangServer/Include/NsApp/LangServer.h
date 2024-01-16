////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_LANGSERVER_H__
#define __APP_LANGSERVER_H__


#include <NsCore/Ptr.h>
#include <NsGui/Stream.h>
#include <NsGui/UIElement.h>
#include <NsApp/LangServerApi.h>
#include <NsCore/StringFwd.h>


namespace Noesis
{
struct Uri;
class UIElement;

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251 4275)

typedef void (*RenderCallback)(UIElement* content, int renderWidth, int renderHeight,
    double renderTime, const char* savePath);
typedef void (*LangServerUriCallback)(const Uri& message, BaseString& buffer);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// A XAML language server which implements the `Language Server Protocol
/// <https://microsoft.github.io/language-server-protocol/>`_
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_LANGSERVER_API LangServer
{
public:
    static void RunExclusive();
    static void Run();
    static void Tick();
    static void Shutdown();
    static void SetRenderCallback(RenderCallback renderCallback);
    static void SetDetails(const char* serverName, int serverPriority);
    static Ptr<Stream> GetXaml(Uri uri, bool loadFile);
    static bool FolderHasFontFamily(const Uri& folder, const char* familyName);
private:
    LangServer() = default;
};

NS_WARNING_POP

};


#endif