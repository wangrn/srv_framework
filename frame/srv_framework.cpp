#include <sys/epoll.h>

#include "srv_framework.h"

enum SocketStat {
    SOCKET_UNUSED,
    SOCKET_TCP_ACCEPT,  //Accept出来的socket
    SOCKET_TCP_LISTEN,  //TCP服务端
    SOCKET_TCP_CONNECTING, //TCP客户端
    SOCKET_TCP_CONNECTED,  //TCP客户端
    SOCKET_TCP_RECONNECT_WAIT,
    SOCKET_UDP  // UDP服务端 
};

struct SocketContext{
    int iSocket;
    struct sockaddr_in stClientAddr;
    
    time_t tCreateTime;
    time_t tLastAccessTime;
    SocketStat Stat;//SCTX的定义到此
    int iBytesRecved;
    int iBytesToSend;
    int iPkgLen;
    char RecvBuf[RECVBUF_LEN];  
    char SendBuf[SENDBUF_LEN];
    int iSrvName; // 本地监听的Name；如果是本身作为客户端时，在客户端列表的索引    
    int iPkgHeadLen;

    SocketContext()
    {
        Stat = SOCKET_UDP;
    }
};


#define MAX_SOCKET_EVENTS 256
SrvFrameWork::SrvFrameWork()
{
    _epollfd = epoll_create(MAX_SOCKET_EVENTS);
    _stop = false;
}

SrvFrameWork::~SrvFrameWork()
{
    close(_epollfd);
}

int SrvFrameWork::Start()
{
    struct epoll_event events[MAX_SOCKET_EVENTS];
    int timeout = -1;

    while(!_stop) {
        int nfds = epoll_wait(_epollfd, events, MAX_SOCKET_EVENTS, timeout);
        for (int i = 0; i < nfds; i ++) {
            int socket = events[i].data.fd;

            if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                HandleErr(socket);
            }
            if ((events[i].events & EPOLLIN) != 0) {
                HandleRead(socket);
            }
            if ((events[i].events & EPOLLOUT) != 0) {
                HandleWrite(socket);
            }
        }
        sleep(1);
    }
    return 0;
}

/**
 * 将服务iSvrName绑定到ip:port的UDP上, 如果收到数据包, 回调handle进行处理.
 */
int SrvFrameWork::AddUdpSvrHandle(int iSrvName, char *ip, uint16_t port, UdpSrvHandle handle)
{
    UdpSrv *srv = new UdpSrv(ip, port, iSrvName, handle);
    SocketContext *ctx = new SocketContext();
    ctx->iSocket = CreateUdpSocket(ip, port);
    ctx->iSrvName = iSrvName;

    _svrMap[iSrvName] = srv;
    _sockMap[ctx->iSocket] = (void*)ctx;

    srv->iSocket = ctx->iSocket;

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = ctx->iSocket;
    int ret = epoll_ctl(_epollfd, EPOLL_CTL_ADD, ctx->iSocket, &ev);

    fprintf(stderr, "Add sock%d to epoll, ret=%d\n", ctx->iSocket, ret);
    return ret;
}

int SrvFrameWork::SendUdp(int iSrvName, const struct sockaddr_in *pstAddr, void *pPkg, int iPkgLen)
{
    if(pPkg == NULL || iPkgLen <= 0 || iPkgLen > SENDBUF_LEN)
    {
        return -1;
    }

    SvrNameSvrMap::iterator it2 = _svrMap.find(iSrvName);
    if (it2 == _svrMap.end()) return -2;

    SocketSvrMap::iterator it = _sockMap.find(it2->second->iSocket);
    if (it == _sockMap.end()) return -3;
    SocketContext *ctx = (SocketContext *)it->second;

    int iSocket = ctx->iSocket;
    int iAddrLen = sizeof(*pstAddr);
    int iRet = sendto(iSocket, pPkg, iPkgLen, 0, (const struct sockaddr *) pstAddr, iAddrLen);

    if(iRet != iPkgLen)
    {
        return -iRet;
    }

    return 0;
}

int SrvFrameWork::HandleErr(int socket)
{
    //TODO
    return 0;
}

int SrvFrameWork::HandleRead(int socket)
{
    SocketSvrMap::iterator it = _sockMap.find(socket);
    if (it == _sockMap.end()) return -1;

    SocketContext *ctx = (SocketContext*)it->second;
    assert(socket == ctx->iSocket);

    int iAddrLen = sizeof(ctx->stClientAddr);
    int ret = recvfrom(socket, ctx->RecvBuf, sizeof(ctx->RecvBuf), 0,
            (struct sockaddr*)&(ctx->stClientAddr), (socklen_t *)&iAddrLen);
    if (ret <= 0) {
        return -2;
    }

    SvrNameSvrMap::iterator it2 = _svrMap.find(ctx->iSrvName);
    if (it2 == _svrMap.end()) return -3;

    it2->second->handle((SvrPkgContext*)ctx, ctx->RecvBuf, ret);

    return 0;
}

int SrvFrameWork::HandleWrite(int socket)
{
    //TODO
    return 0;
}

