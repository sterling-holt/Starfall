////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_SOCKET_H__
#define __APP_SOCKET_H__


#include <NsCore/Noesis.h>
#include <NsCore/StringFwd.h>


#ifdef NS_PLATFORM_WINDOWS
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#endif

#ifdef NS_PLATFORM_SCE
    #include <net.h>
#endif

#if defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
    #include <netinet/in.h>
#endif

#ifdef NS_PLATFORM_NX
    #include <nn/socket.h>
#endif


namespace Noesis
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Berkeley stream socket implementation
////////////////////////////////////////////////////////////////////////////////////////////////////
class LangServerSocket
{
public:
    /// Initializes the socket system
    static void Init();

    /// Sutdowns the socket system
    static void Shutdown();

    /// Gets the IP address of this computer
    static bool GetLocalIP(BaseString& ip);

    /// Returns whether there is an active connection
    bool Connected() const;

    /// Closes the active connection
    void Disconnect();

    /// Gets the number of bytes that are immediately available for reading
    bool PendingBytes();

    /// Establishes a TCP connection to a specified server
    void Connect(const char* address, uint32_t port);

    /// Sends data to a TCP connection
    bool Send(const char* buffer, uint32_t len);

    /// Receives data from a TCP connection
    bool Recv(char* buffer, uint32_t len);

    /// Starts listening for incomming TCP connections on the specified local address
    void Listen(const char* address, uint32_t port);

    /// Accepts an incomming TCP connection on a listening socket
    bool Accept(LangServerSocket&);

    /// Accepts an incoming TCP connection on a listening socket, blocks if waiting for a connection
    bool AcceptBlock(LangServerSocket& socket);

    /// Opens the UDP socket for broadcasting
    void Open();

    /// Broadcasts the buffer data to the specified port through the UDP socekt
    bool Broadcast(const char* buffer, uint32_t len, uint32_t port);

    /// Receives data from the UDP socket
    bool RecvFrom(char* buffer, uint32_t len, BaseString& address, uint32_t& bytesReceived);

    /// Starts listening for incomming UDP datagrams on the specified local address
    void Listen(uint32_t port);

private:
    /// Maximum number of incoming connections to queue
    static const uint32_t ListenBacklog = 16;

#if defined(NS_PLATFORM_WINDOWS)
    SOCKET mSocket = INVALID_SOCKET;
#elif defined(NS_PLATFORM_SCE)
    SceNetId mSocket = -1;
    SceNetId mEpoll = -1;
#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
    int mSocket = -1;
#elif defined(NS_PLATFORM_NX)
    int mSocket = -1;
#endif
};

}

#include "LangServerSocket.inl"

#endif
