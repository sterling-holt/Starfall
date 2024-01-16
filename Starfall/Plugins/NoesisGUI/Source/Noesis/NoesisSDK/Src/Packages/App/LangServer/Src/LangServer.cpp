////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/LangServer.h>
#include <NsCore/HighResTimer.h>
#include <NsCore/Log.h>
#include <NsCore/String.h>
#include <NsGui/Stream.h>
#include <NsGui/MemoryStream.h>

#include "DocumentHelper.h"
#include "LangServerSocket.h"
#include "MessageBuffer.h"
#include "MessageReader.h"
#include "Workspace.h"


#ifdef NS_COMPILER_MSVC
#define NS_STDCALL __stdcall
#define NS_EXPORT
#endif

#ifdef NS_COMPILER_GCC
#define NS_STDCALL
#define NS_EXPORT __attribute__ ((visibility("default")))
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Common private definitions
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
constexpr int TcpPortRangeBegin = 16629;
constexpr int TcpPortRangeEnd = 16649;
constexpr int UdpPortRangeBegin = 16629;
constexpr int UdpPortRangeEnd = 16649;
constexpr char ServerAddress[] = "127.0.0.1";

Noesis::LangServerSocket gListenSocket;
Noesis::LangServerSocket gDataSocket;
Noesis::LangServerSocket gBroadcastSocket;
Noesis::Workspace gWorkspace;
char gReceiveBuffer;
Noesis::MessageBuffer gMessageBuffer;
Noesis::FixedString<2048> gSendBuffer;
Noesis::FixedString<254> gAnnouncementMessage;
Noesis::FixedString<64> gServerName = "Unknown";
int gServerPriority = 0;
bool gIsRunning;
double gLastBroadcastTime;
uint64_t gStartTicks;
bool gIsConnected;
Noesis::RenderCallback gRenderCallback = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////
void LangServerInit()
{
    Noesis::LangServerSocket::Init();

    int serverPort = TcpPortRangeBegin;
    do
    {
        gListenSocket.Listen(ServerAddress, serverPort);
    } while (!gListenSocket.Connected() && serverPort++ < TcpPortRangeEnd);

    if (!gListenSocket.Connected())
    {
        NS_LOG_INFO("LangServer failed to bind to socket");
        return;
    }

    NS_LOG_INFO("LangServer listening on socket: %s:%d", ServerAddress, serverPort);

    gAnnouncementMessage = { Noesis::FixedString<254>::VarArgs(),
        "{\"serverPort\":%u,\"serverName\":\"%s\",\"serverPriority\":%d}",
        serverPort, gServerName.Str(), gServerPriority };

    gBroadcastSocket.Open();

    gLastBroadcastTime = 0;

    gIsRunning = true;
    gIsConnected = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void TrySendMessages(Noesis::FixedString<2048>& sendBuffer, Noesis::LangServerSocket& dataSocket)
{
    if (!sendBuffer.Empty())
    {
        //NS_LOG_INFO("### Send message\n%s\n###\n\n", sendBuffer.Str());
        dataSocket.Send(sendBuffer.Str(), sendBuffer.Size());
        sendBuffer.Clear();
    }
}
}

#ifdef NS_MANAGED
////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" NS_EXPORT void NS_STDCALL Noesis_RunLangServer()
{
#ifdef NS_PLATFORM_WINDOWS
#pragma comment(linker, "/export:Noesis_RunLangServer=" __FUNCDNAME__)
#endif
    Noesis::LangServer::Run();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" NS_EXPORT void NS_STDCALL Noesis_UpdateLangServer()
{
#ifdef NS_PLATFORM_WINDOWS
#pragma comment(linker, "/export:Noesis_UpdateLangServer=" __FUNCDNAME__)
#endif
    Noesis::LangServer::Tick();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" NS_EXPORT void NS_STDCALL Noesis_ShutdownLangServer()
{
#ifdef NS_PLATFORM_WINDOWS
#pragma comment(linker, "/export:Noesis_ShutdownLangServer=" __FUNCDNAME__)
#endif
    Noesis::LangServer::Shutdown();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" NS_EXPORT char* NS_STDCALL Noesis_LangServerGetXaml(char* uri)
{
#ifdef NS_PLATFORM_WINDOWS
#pragma comment(linker, "/export:Noesis_LangServerGetXaml=" __FUNCDNAME__)
#endif

    NS_ASSERT(!Noesis::StrIsNullOrEmpty(uri));

    const Noesis::Uri uri_(uri);    
    if (!gWorkspace.IsInitialized())
    {
        NS_ERROR("Cannot load xaml before LangServer is initialised");
        return nullptr;
    }
    if (!gWorkspace.HasDocument(uri_))
    {
        return nullptr;
    }

    Noesis::DocumentContainer& document = gWorkspace.GetDocument(uri_);
    char* result = (char*)Noesis::Alloc(document.text.Size() + 1);
    Noesis::StrCopy(result, document.text.Size() + 1, document.text.Str());
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" NS_EXPORT void NS_STDCALL Noesis_SetLangServerDetails(const char* serverName, 
    int serverPriority)
{
#ifdef NS_PLATFORM_WINDOWS
#pragma comment(linker, "/export:Noesis_SetLangServerDetails=" __FUNCDNAME__)
#endif

    Noesis::LangServer::SetDetails(serverName, serverPriority);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<Noesis::Stream> Noesis::LangServer::GetXaml(Uri uri, bool loadFile)
{
    NS_ASSERT(!StrIsNullOrEmpty(uri.Str()));

    const Uri uri_(uri);
    if (!gWorkspace.IsInitialized())
    {
        NS_ERROR("Cannot load xaml before LangServer is initialised");
        return nullptr;
    }

    if (!gWorkspace.HasDocument(uri_))
    {
        if (!loadFile)
        {
            return nullptr;
        }
        FixedString<512> path;
        uri.GetPath(path);
        return OpenFileStream(path.Str());
    }

    const DocumentContainer& document = gWorkspace.GetDocument(uri_);
    return *new MemoryStream(document.text.Str(), document.text.Size());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Noesis::LangServer::FolderHasFontFamily(const Uri& folder, const char* familyName)
{
    return DocumentHelper::FolderHasFontFamily(folder, familyName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::LangServer::SetRenderCallback(RenderCallback renderCallback)
{
    gRenderCallback = renderCallback;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::LangServer::SetDetails(const char* serverName, int serverPriority)
{
    if (gIsRunning)
    {
        NS_ERROR("LangServer details cannot be set while running");
        return;
    }
    NS_ASSERT(!StrIsNullOrEmpty(serverName));
    gServerName = serverName;
    gServerPriority = serverPriority;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::LangServer::RunExclusive()
{
    LangServerInit();
    while (gIsRunning)
    {
        const double currentTime = HighResTimer::Seconds(HighResTimer::Ticks() - gStartTicks);
        if (!gDataSocket.Connected())
        {
            if (currentTime > gLastBroadcastTime + 0.25)
            {
                gLastBroadcastTime = currentTime;
                for (int broadcastPort = UdpPortRangeBegin; broadcastPort < UdpPortRangeEnd;
                    ++broadcastPort)
                {
                    gBroadcastSocket.Broadcast(gAnnouncementMessage.Str(),
                        gAnnouncementMessage.Size(), broadcastPort);
                }
            }

            gListenSocket.Accept(gDataSocket);

            continue;
        }

        while (gDataSocket.Recv(&gReceiveBuffer, 1))
        {
            gMessageBuffer.HandleChar(gReceiveBuffer);

            if (gMessageBuffer.IsMessageCompleted())
            {
                const JsonObject body = gMessageBuffer.GetBody();

                gMessageBuffer.Clear();

                const char* method = body.GetStrValue("method");
                if (StrEquals(method, "exit"))
                {
                    gIsRunning = false;
                    break;
                }

                MessageReader::HandleMessage(gRenderCallback, body, gWorkspace,
                    currentTime, gSendBuffer);

                TrySendMessages(gSendBuffer, gDataSocket);
            }
        }
    }

    Shutdown();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::LangServer::Run()
{
    LangServerInit(); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::LangServer::Tick()
{
    if (!gIsRunning)
    {
        return;
    }

    const double currentTime = HighResTimer::Seconds(HighResTimer::Ticks() - gStartTicks);
    if (!gDataSocket.Connected())
    {
        if (currentTime > gLastBroadcastTime + 0.1)
        {
            gLastBroadcastTime = currentTime;
            //NS_LOG_INFO("-> Broadcast announcement %f\n\n", currentTime);
            for (int broadcastPort = UdpPortRangeBegin; broadcastPort < UdpPortRangeEnd;
                ++broadcastPort)
            {
                gBroadcastSocket.Broadcast(gAnnouncementMessage.Str(),
                    gAnnouncementMessage.Size(), broadcastPort);
            }
        }

        if (gIsConnected)
        {
            gIsConnected = false;
            NS_LOG_INFO("LangServer client disconnected");
        }

        if (!gListenSocket.PendingBytes())
        {
            return;
        }

        gListenSocket.Accept(gDataSocket);

        gIsConnected = true;
        gLastBroadcastTime = 0;

        NS_LOG_INFO("LangServer client connected");

        return;
    }

    while (gDataSocket.PendingBytes() && gDataSocket.Recv(&gReceiveBuffer, 1))
    {
        gMessageBuffer.HandleChar(gReceiveBuffer);

        if (gMessageBuffer.IsMessageCompleted())
        {
            const JsonObject body = gMessageBuffer.GetBody();

            gMessageBuffer.Clear();

            const char* method = body.GetStrValue("method");
            if (StrEquals(method, "exit"))
            {
                Shutdown();
            }

            MessageReader::HandleMessage(gRenderCallback, body, gWorkspace, currentTime,
                gSendBuffer);
            TrySendMessages(gSendBuffer, gDataSocket);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::LangServer::Shutdown()
{
    if (!gIsRunning)
    {
        return;
    }
    LangServerSocket::Shutdown();

    gIsRunning = false;

    gWorkspace.Clear();
    gListenSocket.Disconnect();
    gDataSocket.Disconnect();
    gBroadcastSocket.Disconnect();
}