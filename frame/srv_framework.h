// wangrn wang_rn@163.com
#ifndef SRV_FRAMEWORK_H_
#define SRV_FRAMEWORK_H_
#include <iostream>
#include <map>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
using namespace std;

#include "utils.h"

const int MAX_IP_LEN = 32;
const int MAX_SERVER_NUM = 64;

struct SvrPkgContext;
typedef int (*UdpSrvHandle)(SvrPkgContext *pstSctx, char *sPkg, int iPkgLen);


/**
 * 注册一个UDP的服务端口
 */
struct UdpSrv
{
    char sServerIP[MAX_IP_LEN];  //本机IP
    uint16_t wServerPort;        //监听的port

    uint32_t dwServerName;       //服务的序列号.
    UdpSrvHandle handle;

    int iSocket;
    UdpSrv(const char *ip, uint16_t port, uint32_t srvname, UdpSrvHandle callback)
    {
        strcpy(sServerIP, ip);
        wServerPort = port;
        dwServerName = srvname;
        handle = callback;
    }
};

/**
 * 收到一个包后, 回掉给应用的context信息.
 */
struct SvrPkgContext
{
    const int iSocket;   //socket
    const struct sockaddr_in stClientAddr; //发包端的信息,用于回包.
};



class SrvFrameWork
{
public:
    SrvFrameWork();
    ~SrvFrameWork();

    int Start();
    void Stop() {_stop=true;}

    //以iSrvName的端口, 向pstAddr发送数据包.
    int SendUdp(int iSrvName, const struct sockaddr_in *pstAddr, void *pPkg, int iPkgLen);

    int AddUdpSvrHandle(int iSrvName, char *ip, uint16_t port, UdpSrvHandle handle);

protected:
    int HandleErr(int socket);
    int HandleRead(int socket);
    int HandleWrite(int socket);

private:
    bool _stop;
    int _epollfd;
    typedef std::map<int,      void*> SocketSvrMap;
    typedef std::map<uint32_t, UdpSrv*> SvrNameSvrMap;

    SocketSvrMap _sockMap;
    SvrNameSvrMap _svrMap;
};

#endif //SRV_FRAMEWORK_H_
