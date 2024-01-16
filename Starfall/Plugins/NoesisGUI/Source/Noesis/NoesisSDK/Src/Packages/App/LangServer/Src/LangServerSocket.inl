////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsCore/Error.h>
#include <NsCore/String.h>


#ifdef NS_PLATFORM_NX
    #include <nn/nifm/nifm_ApiIpAddress.h>
    #include <nn/nifm/nifm_Api.h>
#endif

#if defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <sys/poll.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#ifdef NS_PLATFORM_SCE
    #define NS_SOCKET_NAME "Noesis::LangServer"
#endif


namespace Noesis
{

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void LangServerSocket::Init()
{
#if defined(NS_PLATFORM_WINDOWS)
    WSADATA wsadata;
    int error = WSAStartup(MAKEWORD(2,2), &wsadata);
    NS_ASSERT(error == 0);
#elif defined (NS_PLATFORM_NX)
    static nn::socket::ConfigDefaultWithMemory SocketConfigWithMemory;
    nn::Result r = nn::socket::Initialize(SocketConfigWithMemory);
    NS_ASSERT(r.IsSuccess());
    r = nn::nifm::Initialize();
    NS_ASSERT(r.IsSuccess());
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void LangServerSocket::Shutdown()
{
#ifdef NS_PLATFORM_WINDOWS
    WSACleanup();
#elif defined (NS_PLATFORM_NX)
    nn::socket::Finalize();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LangServerSocket::GetLocalIP(Noesis::BaseString& ip)
{
#ifdef NS_PLATFORM_WINDOWS
    char hostname[256];
    if (!gethostname(hostname, sizeof(hostname)))
    {
        ADDRINFOA host{};
        host.ai_family = AF_INET;
        host.ai_socktype = SOCK_STREAM;
        host.ai_protocol = IPPROTO_TCP;

        ADDRINFOA* result = NULL;

        if (!getaddrinfo(hostname, NULL, &host, &result))
        {
            if (result)
            {
                char str[16];
                IN_ADDR* addr = &((SOCKADDR_IN*)result->ai_addr)->sin_addr;
                if (inet_ntop(AF_INET, addr, str, 16))
                {
                    ip = str;
                    return true;
                }
            }
        }
    }
#endif

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LangServerSocket::Connected() const
{
#if defined(NS_PLATFORM_WINDOWS)
    return mSocket != INVALID_SOCKET;
#elif defined(NS_PLATFORM_SCE)
    return mSocket != -1;
#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
    return mSocket != -1;
#elif defined (NS_PLATFORM_NX)
    return mSocket != -1;
#else
    return false;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void LangServerSocket::Connect(const char* address, uint32_t port)
{
    NS_ASSERT(!Connected());

#if defined(NS_PLATFORM_WINDOWS)
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    NS_ASSERT(mSocket != INVALID_SOCKET);

    // Increase the send buffer to 1MB
    int size = 1024 * 1024;
    int error = setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(int));
    NS_ASSERT(error == 0);

    sockaddr_in sock = {};
    sock.sin_family = AF_INET;
    inet_pton(AF_INET, address, &sock.sin_addr);
    sock.sin_port = htons((u_short)port);

    if (connect(mSocket, (sockaddr*)&sock, sizeof(sock)) == SOCKET_ERROR)
    {
        Disconnect();
    }

#elif defined(NS_PLATFORM_SCE)
    mSocket = sceNetSocket(NS_SOCKET_NAME, SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);
    NS_ASSERT(mSocket >= 0);

    mEpoll = sceNetEpollCreate(NS_SOCKET_NAME, 0);
    NS_ASSERT(mEpoll >= 0);

    SceNetSockaddrIn sock = {};
    sock.sin_len = sizeof(sock);
    sock.sin_family = SCE_NET_AF_INET;
    sceNetInetPton(SCE_NET_AF_INET, address, &sock.sin_addr);
    sock.sin_port = sceNetHtons(port);

    if (sceNetConnect(mSocket, (const SceNetSockaddr*)&sock, sizeof(sock)) != 0)
    {
        Disconnect();
    }

    SceNetEpollEvent poll = { SCE_NET_EPOLLIN };
    if (sceNetEpollControl(mEpoll, SCE_NET_EPOLL_CTL_ADD, mSocket, &poll) != 0)
    {
        Disconnect();
    }

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    NS_ASSERT(mSocket != -1);

    // Increase the send buffer to 1MB
    int size = 1024 * 1024;
    int error = setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(int));
    NS_ASSERT(error == 0);

    sockaddr_in sock = {};
    sock.sin_family = AF_INET;
    inet_pton(AF_INET, address, &sock.sin_addr);
    sock.sin_port = htons((short)port);

    if (connect(mSocket, (sockaddr*)&sock, sizeof(sock)) == -1)
    {
        Disconnect();
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void LangServerSocket::Disconnect()
{
    if (Connected())
    {
#if defined(NS_PLATFORM_WINDOWS)
        int error = closesocket(mSocket);
        NS_ASSERT(error == 0);
        mSocket = INVALID_SOCKET;

#elif defined(NS_PLATFORM_SCE)
        int error = sceNetEpollDestroy(mEpoll);
        NS_ASSERT(error == 0);
        mEpoll = -1;
        error = sceNetSocketClose(mSocket);
        NS_ASSERT(error == 0);
        mSocket= -1;

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
        int error = close(mSocket);
        NS_ASSERT(error == 0);
        mSocket= -1;

#elif defined(NS_PLATFORM_NX)
        int error = nn::socket::Close(mSocket);
        NS_ASSERT(error == 0);
        mSocket= -1;
#endif
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LangServerSocket::Send(const char* buffer, uint32_t len)
{
    if (Connected())
    {
#if defined(NS_PLATFORM_WINDOWS)
        int sent = send(mSocket, buffer, len, 0);
        if (sent == SOCKET_ERROR || (uint32_t)sent != len)
        {
            Disconnect();
            return false;
        }

        return true;

#elif defined(NS_PLATFORM_SCE)
        int sent = sceNetSend(mSocket, buffer, len, 0);
        if (sent < 0 || (uint32_t)sent != len)
        {
            Disconnect();
            return false;
        }

        return true;

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
        ssize_t sent = send(mSocket, buffer, len, 0);
        if (sent == -1 || (uint32_t)sent != len)
        {
            Disconnect();
            return false;
        }

        return true;

#elif defined(NS_PLATFORM_NX)
        ssize_t sent = nn::socket::Send(mSocket, buffer, len, nn::socket::MsgFlag::Msg_None);
        if (sent == -1 || (uint32_t)sent != len)
        {
            Disconnect();
            return false;
        }

        return true;
#endif
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LangServerSocket::Recv(char* buffer, uint32_t len)
{
    if (Connected())
    {
#if defined(NS_PLATFORM_WINDOWS)
        do
        {
            int received = recv(mSocket, buffer, len, 0);
            if (received == SOCKET_ERROR)
            {
                Disconnect();
                return false;
            }
            buffer += received;
            len -= (uint32_t)received;
        } while (len > 0);

        return true;

#elif defined(NS_PLATFORM_SCE)
        do
        {
            int received = sceNetRecv(mSocket, buffer, len, 0);
            if (received < 0)
            {
                Disconnect();
                return false;
            }
            buffer += received;
            len -= (uint32_t)received;
        } while (len > 0);

        return true;

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
        do
        {
            ssize_t received = recv(mSocket, buffer, len, 0);
            if (received == -1)
            {
                Disconnect();
                return false;
            }
            buffer += received;
            len -= (uint32_t)received;
        } while (len > 0);

        return true;

#elif defined(NS_PLATFORM_NX)
        do
        {
            ssize_t received = nn::socket::Recv(mSocket, buffer, len, nn::socket::MsgFlag::Msg_None);
            if (received == -1)
            {
                Disconnect();
                return false;
            }
            buffer += received;
            len -= (uint32_t)received;
        }
        while (len > 0);

        return true;
#endif
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LangServerSocket::PendingBytes()
{
    if (Connected())
    {
#ifdef NS_PLATFORM_WINDOWS
        WSAPOLLFD poll;
        poll.fd = mSocket;
        poll.events = POLLRDNORM;
        poll.revents = 0;
        if (WSAPoll(&poll, 1, 0) == SOCKET_ERROR || (poll.revents & (POLLERR | POLLHUP)) != 0)
        {
            Disconnect();
            return false;
        }
        return (poll.revents & POLLRDNORM) != 0;

#elif defined(NS_PLATFORM_SCE)
        SceNetEpollEvent poll;
        poll.events = 0;
        poll.reserved = 0;
        if (sceNetEpollWait(mEpoll, &poll, 1, 0) < 0 || (poll.events & (SCE_NET_EPOLLERR | SCE_NET_EPOLLHUP)) != 0)
        {
            Disconnect();
            return false;
        }

        return (poll.events & SCE_NET_EPOLLIN) != 0;

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
        pollfd poll;
        poll.fd = mSocket;
        poll.events = POLLRDNORM;
        poll.revents = 0;
        if (::poll(&poll, 1, 0) == -1 || (poll.revents & (POLLERR | POLLHUP)) != 0)
        {
            Disconnect();
            return false;
        }

        return (poll.revents & POLLRDNORM) != 0;

#elif defined(NS_PLATFORM_NX)
        nn::socket::PollFd poll;
        poll.fd = mSocket;
        poll.events = nn::socket::PollEvent::PollRdNorm;
        poll.revents = nn::socket::PollEvent::PollNone;

        if (nn::socket::Poll(&poll, 1, 0) == -1 || ((int)poll.revents &
            ((int)nn::socket::PollEvent::PollErr | (int)nn::socket::PollEvent::PollHup)) != 0)
        {
            Disconnect();
            return false;
        }

        return ((int)poll.revents & (int)nn::socket::PollEvent::PollRdNorm) != 0;
#endif
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void LangServerSocket::Listen(const char* address, uint32_t port)
{
    NS_ASSERT(!Connected());

#if defined(NS_PLATFORM_WINDOWS)
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    NS_ASSERT(mSocket != INVALID_SOCKET);

    sockaddr_in sock = {};
    sock.sin_family = AF_INET;
    inet_pton(AF_INET, address, &sock.sin_addr);
    sock.sin_port = htons((u_short)port);

    if (bind(mSocket, (sockaddr*)&sock, sizeof(sock)) == SOCKET_ERROR)
    {
        Disconnect();
    }

    if (listen(mSocket, (int)ListenBacklog) == SOCKET_ERROR)
    {
        Disconnect();
    }

#elif defined(NS_PLATFORM_SCE)
    mSocket = sceNetSocket(NS_SOCKET_NAME, SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);
    NS_ASSERT(mSocket >= 0);

    mEpoll = sceNetEpollCreate(NS_SOCKET_NAME, 0);
    NS_ASSERT(mEpoll >= 0);

    SceNetSockaddrIn sock = {};
    sock.sin_len = sizeof(sock);
    sock.sin_family = SCE_NET_AF_INET;
    sceNetInetPton(SCE_NET_AF_INET, address, &sock.sin_addr);
    sock.sin_port = sceNetHtons(port);

    if (sceNetBind(mSocket, (const SceNetSockaddr*)&sock, sizeof(sock)) != 0)
    {
        Disconnect();
    }

    if (sceNetListen(mSocket, (int)ListenBacklog) != 0)
    {
        Disconnect();
    }

    SceNetEpollEvent poll = { SCE_NET_EPOLLIN };
    if (sceNetEpollControl(mEpoll, SCE_NET_EPOLL_CTL_ADD, mSocket, &poll) != 0)
    {
        Disconnect();
    }

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    NS_ASSERT(mSocket != -1);

    sockaddr_in sock = {};
    sock.sin_family = AF_INET;
    inet_pton(AF_INET, address, &sock.sin_addr);
    sock.sin_port = htons((u_short)port);

    if (bind(mSocket, (sockaddr*)&sock, sizeof(sock)) == -1)
    {
        Disconnect();
    }

    if (listen(mSocket, (int)ListenBacklog) == -1)
    {
        Disconnect();
    }

#elif defined (NS_PLATFORM_NX)
    mSocket = nn::socket::Socket(nn::socket::Family::Af_Inet, nn::socket::Type::Sock_Stream,
        (nn::socket::Protocol)nn::socket::Type::Sock_Default);
    NS_ASSERT(mSocket != -1);

    nn::socket::SockAddrIn sock = {};
    sock.sin_family = nn::socket::Family::Af_Inet;
    sock.sin_port = nn::socket::InetHtons((uint16_t)port);
    nn::socket::InetPton(nn::socket::Family::Af_Inet, address, &sock.sin_addr);

    if (nn::socket::Bind(mSocket, (nn::socket::SockAddr*)&sock, sizeof(sock)) == -1)
    {
        Disconnect();
    }

    if (nn::socket::Listen(mSocket, ListenBacklog) == -1)
    {
        Disconnect();
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LangServerSocket::Accept(LangServerSocket& socket)
{
    if (!PendingBytes())
    {
        return false;
    }

    if (socket.Connected())
    {
        return false;
    }

    return AcceptBlock(socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LangServerSocket::AcceptBlock(LangServerSocket& socket)
{
#if defined(NS_PLATFORM_WINDOWS)
    socket.mSocket = accept(mSocket, nullptr, nullptr);
#elif defined(NS_PLATFORM_SCE)
    socket.mSocket = sceNetAccept(mSocket, nullptr, nullptr);

    socket.mEpoll = sceNetEpollCreate(NS_SOCKET_NAME, 0);
    NS_ASSERT(socket.mEpoll >= 0);

    SceNetEpollEvent poll = { SCE_NET_EPOLLIN };
    if (sceNetEpollControl(socket.mEpoll, SCE_NET_EPOLL_CTL_ADD, socket.mSocket, &poll) != 0)
    {
        Disconnect();
    }

#elif defined(NS_PLATFORM_APPLE)
    socket.mSocket = accept(mSocket, nullptr, nullptr);
    int option_value = 1;
    setsockopt(socket.mSocket, SOL_SOCKET, SO_NOSIGPIPE, &option_value, sizeof(option_value));
#elif defined(NS_PLATFORM_LINUX)
    socket.mSocket = accept(mSocket, nullptr, nullptr);
#elif defined(NS_PLATFORM_ANDROID)
    socket.mSocket = accept(mSocket, nullptr, nullptr);
#elif defined(NS_PLATFORM_NX)
    socket.mSocket = nn::socket::Accept(mSocket, (nn::socket::SockAddr*)nullptr, nullptr);
#endif

    return socket.Connected();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void LangServerSocket::Open()
{
    NS_ASSERT(!Connected());

#if defined(NS_PLATFORM_WINDOWS)
    mSocket = socket(AF_INET, SOCK_DGRAM, 0);
    NS_ASSERT(mSocket != INVALID_SOCKET);

    int broadcastOn = 1;
    int error = setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcastOn, sizeof(broadcastOn));
    NS_ASSERT(error == 0);

#elif defined(NS_PLATFORM_SCE)
    mSocket = sceNetSocket(NS_SOCKET_NAME, SCE_NET_AF_INET, SCE_NET_SOCK_DGRAM, 0);
    NS_ASSERT(mSocket >= 0);

    mEpoll = sceNetEpollCreate(NS_SOCKET_NAME, 0);
    NS_ASSERT(mEpoll >= 0);

    int broadcastOn = 1;
    int error = sceNetSetsockopt(mSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_BROADCAST, (const char*)&broadcastOn, sizeof(broadcastOn));
    NS_ASSERT(error == 0);

    SceNetEpollEvent poll = { SCE_NET_EPOLLIN };
    if (sceNetEpollControl(mEpoll, SCE_NET_EPOLL_CTL_ADD, mSocket, &poll) != 0)
    {
        Disconnect();
    }

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
    mSocket = socket(AF_INET, SOCK_DGRAM, 0);
    NS_ASSERT(mSocket != -1);

    int broadcastOn = 1;
    int error = setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcastOn, sizeof(broadcastOn));
    NS_ASSERT(error == 0);

#elif defined(NS_PLATFORM_NX)
    mSocket = nn::socket::Socket(nn::socket::Family::Af_Inet, nn::socket::Type::Sock_Dgram,
        (nn::socket::Protocol)nn::socket::Type::Sock_Default);
    NS_ASSERT(mSocket != -1);

    int broadcastOn = 1;
    int error = nn::socket::SetSockOpt(mSocket, nn::socket::Level::Sol_Socket,
        nn::socket::Option::So_Broadcast, &broadcastOn, sizeof(broadcastOn));
    NS_ASSERT(error == 0);

#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LangServerSocket::Broadcast(const char* buffer, uint32_t len, uint32_t port)
{
    if (Connected())
    {
#if defined(NS_PLATFORM_WINDOWS)
        sockaddr_in sock = {};
        sock.sin_family = AF_INET;
        sock.sin_addr.s_addr = INADDR_BROADCAST;
        sock.sin_port = htons((u_short)port);

        int sent = sendto(mSocket, buffer, len, 0, (sockaddr*)&sock, sizeof(sock));
        if (sent == SOCKET_ERROR || (uint32_t)sent != len)
        {
            Disconnect();
            return false;
        }

        return true;

#elif defined(NS_PLATFORM_SCE)
        SceNetSockaddrIn sock = {};
        sock.sin_len = sizeof(sock);
        sock.sin_family = SCE_NET_AF_INET;
        sock.sin_addr.s_addr = SCE_NET_INADDR_BROADCAST;
        sock.sin_port = sceNetHtons(port);

        int sent = sceNetSendto(mSocket, buffer, len, 0, (const SceNetSockaddr*)&sock, sizeof(sock));
        if (sent < 0 || (uint32_t)sent != len)
        {
            Disconnect();
            return false;
        }

        return true;

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
        sockaddr_in sock = {};
        sock.sin_family = AF_INET;
        sock.sin_addr.s_addr = INADDR_BROADCAST;
        sock.sin_port = htons((u_short)port);

        ssize_t sent = sendto(mSocket, buffer, len, 0, (sockaddr*)&sock, sizeof(sock));
        if (sent == -1 || (uint32_t)sent != len)
        {
            Disconnect();
            return false;
        }

        return true;

#elif defined (NS_PLATFORM_NX)
        nn::socket::SockAddrIn sock = {};
        sock.sin_family = nn::socket::Family::Af_Inet;
        sock.sin_port = nn::socket::InetHtons((uint16_t)port);

        // INADDR_BROADCAST (255.255.255.255) not supported in NintendoSDK
        // https://developer.nintendo.com/group/development/g1kr9vj6/forums/english/-/gts_message_boards/thread/282367698
        in_addr inAddrIpAddress;
        in_addr inAddrSubnetMask;
        in_addr inAddrDefaultGateway;
        in_addr inAddrPreferredDns;
        in_addr inAddrAlternateDns;
        nn::nifm::GetCurrentIpConfigInfo(&inAddrIpAddress, &inAddrSubnetMask, &inAddrDefaultGateway,
            &inAddrPreferredDns, &inAddrAlternateDns);
        sock.sin_addr.S_addr = (~inAddrSubnetMask.s_addr) | (inAddrIpAddress.s_addr);
        
        ssize_t sent = nn::socket::SendTo(mSocket, buffer, len, nn::socket::MsgFlag::Msg_None,
            (nn::socket::SockAddr*)&sock, sizeof(sock));
        if (sent == -1 || (uint32_t)sent != len)
        {
            Disconnect();
            return false;
        }

        return true;
#endif
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LangServerSocket::RecvFrom(char* buffer, uint32_t len, Noesis::BaseString& address,
    uint32_t& bytesReceived)
{
    if (Connected())
    {
#if defined(NS_PLATFORM_WINDOWS)
        sockaddr_in sock;
        socklen_t sockLen = sizeof(sock);
        int received = recvfrom(mSocket, buffer, len, 0, (sockaddr*)&sock, &sockLen);
        if (received == SOCKET_ERROR)
        {
            Disconnect();
            return false;
        }

        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sock.sin_addr, addr, INET_ADDRSTRLEN);
        address = addr;
        bytesReceived = (uint32_t)received;

        return true;

#elif defined(NS_PLATFORM_SCE)
        SceNetSockaddrIn sock;
        SceNetSocklen_t sockLen = sizeof(sock);
        int received = sceNetRecvfrom(mSocket, buffer, len, 0, (SceNetSockaddr*)&sock, &sockLen);
        if (received < 0)
        {
            Disconnect();
            return false;
        }

        char addr[SCE_NET_INET_ADDRSTRLEN];
        sceNetInetNtop(SCE_NET_AF_INET, &sock.sin_addr, addr, SCE_NET_INET_ADDRSTRLEN);
        address = addr;
        bytesReceived = (uint32_t)received;

        return true;

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
        sockaddr_in sock;
        socklen_t sockLen = sizeof(sock);
        ssize_t received = recvfrom(mSocket, buffer, len, 0, (sockaddr*)&sock, &sockLen);
        if (received == -1)
        {
            Disconnect();
            return false;
        }

        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sock.sin_addr, addr, INET_ADDRSTRLEN);
        address = addr;
        bytesReceived = (uint32_t)received;

        return true;

#elif defined(NS_PLATFORM_NX)
        nn::socket::SockAddrIn sock;
        socklen_t sockLen = sizeof(sock);
        ssize_t received = nn::socket::RecvFrom(mSocket, buffer, len, nn::socket::MsgFlag::Msg_None,
            (nn::socket::SockAddr*)&sock, &sockLen);
        if (received == -1)
        {
            Disconnect();
            return false;
        }

        char addr[INET_ADDRSTRLEN];
        InetNtop(nn::socket::Family::Af_Inet, &sock.sin_addr, addr, INET_ADDRSTRLEN);
        address = addr;
        bytesReceived = (uint32_t)received;
        return true;

#endif
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
inline void LangServerSocket::Listen(uint32_t port)
{
    NS_ASSERT(!Connected());

#if defined(NS_PLATFORM_WINDOWS)
    mSocket = socket(AF_INET, SOCK_DGRAM, 0);
    NS_ASSERT(mSocket != INVALID_SOCKET);

    sockaddr_in sock = {};
    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = INADDR_ANY;
    sock.sin_port = htons((u_short)port);

    if (bind(mSocket, (sockaddr*)&sock, sizeof(sock)) == SOCKET_ERROR)
    {
        Disconnect();
    }

#elif defined(NS_PLATFORM_SCE)
    mSocket = sceNetSocket(NS_SOCKET_NAME, SCE_NET_AF_INET, SCE_NET_SOCK_DGRAM, 0);
    NS_ASSERT(mSocket >= 0);

    mEpoll = sceNetEpollCreate(NS_SOCKET_NAME, 0);
    NS_ASSERT(mEpoll >= 0);

    SceNetSockaddrIn sock = {};
    sock.sin_len = sizeof(sock);
    sock.sin_family = SCE_NET_AF_INET;
    sock.sin_addr.s_addr = SCE_NET_INADDR_ANY;
    sock.sin_port = sceNetHtons(port);

    if (sceNetBind(mSocket, (const SceNetSockaddr*)&sock, sizeof(sock)) != 0)
    {
        Disconnect();
    }

    SceNetEpollEvent poll = { SCE_NET_EPOLLIN };
    if (sceNetEpollControl(mEpoll, SCE_NET_EPOLL_CTL_ADD, mSocket, &poll) != 0)
    {
        Disconnect();
    }

#elif defined(NS_PLATFORM_APPLE) || defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_ANDROID)
    mSocket = socket(AF_INET, SOCK_DGRAM, 0);
    NS_ASSERT(mSocket != -1);

    sockaddr_in sock = {};
    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = INADDR_ANY;
    sock.sin_port = htons((u_short)port);

    if (bind(mSocket, (sockaddr*)&sock, sizeof(sock)) == -1)
    {
        Disconnect();
    }

#elif defined(NS_PLATFORM_NX)
    mSocket = nn::socket::Socket(nn::socket::Family::Af_Inet, nn::socket::Type::Sock_Dgram,
        (nn::socket::Protocol)nn::socket::Type::Sock_Default);
    NS_ASSERT(mSocket != -1);

    nn::socket::SockAddrIn sock = {};
    sock.sin_family = nn::socket::Family::Af_Inet;
    sock.sin_addr.S_addr = nn::socket::InAddr_Any;
    sock.sin_port = nn::socket::InetHtons((uint16_t)port);

    if (nn::socket::Bind(mSocket, (nn::socket::SockAddr*)&sock, sizeof(sock)) == -1)
    {
        Disconnect();
    }
#endif
}

}
