#include <iostream>
using namespace std;

#include "srv_framework.h"

SrvFrameWork gframe;

enum {
    ECHO_SERVER = 1,
    PING_SERVER = 2,
    PONG_SERVER = 3
};


static char sendbuf[4096];
static int mode;

int EchoSrvHandle(SvrPkgContext *pstSctx, char *sPkg, int iPkgLen)
{
    fprintf(stdout, "ECHO [%s] fd=%d\n", sPkg, pstSctx->iSocket);
    int ret = gframe.SendUdp(ECHO_SERVER, &(pstSctx->stClientAddr), sPkg, iPkgLen);
    return 0;
}

int PingSrvHandle(SvrPkgContext *pstSctx, char *sPkg, int iPkgLen)
{
    fprintf(stdout, "PING [%s] fd=%d\n", sPkg, pstSctx->iSocket);
    //gframe.SendUdp(PING_SERVER, &(pstSctx->stClientAddr), sPkg, iPkgLen);
    return 0;
}

int PongSrvHandle(SvrPkgContext *pstSctx, char *sPkg, int iPkgLen)
{
    fprintf(stdout, "PONG [%s] fd=%d\n", sPkg, pstSctx->iSocket);
    //gframe.SendUdp(PING_SERVER, &(pstSctx->stClientAddr), sPkg, iPkgLen);
    return 0;
}

void print_usage_exit(char *prg)
{
    printf("%s localip localport [remoteip remoteport]\n", prg);
    exit(1);
}
int main(int argc, char *argv[])
{
    if (argc < 3) {
        print_usage_exit(argv[0]);
    }

    mode = 0;
    gframe.AddUdpSvrHandle(ECHO_SERVER, argv[1], atoi(argv[2]), EchoSrvHandle);
    if (argc > 3) {
        mode = 1;
        struct sockaddr_in stSvrSockAddr;
        memset(&stSvrSockAddr, 0, sizeof(sockaddr_in));
        stSvrSockAddr.sin_family = AF_INET;
        stSvrSockAddr.sin_addr.s_addr = inet_addr(argv[3]);
        stSvrSockAddr.sin_port = htons(atoi(argv[4]));
        int iAddrLen = sizeof(stSvrSockAddr);
        
        char buff[4096];
        sprintf(buff, "testtest");
        int len = strlen(buff);
        int ret = gframe.SendUdp(ECHO_SERVER, &stSvrSockAddr, buff, len);
        fprintf(stdout, "send data=%d ret=%d\n", len, ret);
    }
    if (mode == 0) {
        gframe.AddUdpSvrHandle(PING_SERVER, argv[1], atoi(argv[2])+1, PingSrvHandle);
        gframe.AddUdpSvrHandle(PONG_SERVER, argv[1], atoi(argv[2])+2, PongSrvHandle);
    }
    gframe.Start();
    return 0;
}
