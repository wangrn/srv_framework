#include "utils.h"

int CreateUdpSocket(const char *ip, uint16_t port)
{
    struct in_addr addr;
    if (inet_aton(ip, &addr) == 0) {
        return -1; 
    }   
    int s = -1; 
    int flags = 1;              //nonblock reusaddr
    int iRcvBufLen = RECVBUF_LEN + 200;

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s == -1) 
        return -2; 
    if(ioctl(s, FIONBIO, &flags) && ((flags = fcntl(s, F_GETFL, 0)) < 0 || fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0)) 
    {   
        close(s);
        return -3; 
    }   
    
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags)) == -1) 
    {   
    }   

    struct sockaddr_in stAddr;
    memset(&stAddr, 0, sizeof(stAddr));
    stAddr.sin_family = AF_INET;
    stAddr.sin_addr = addr;
    stAddr.sin_port = htons(port);
    if(bind(s, (struct sockaddr *) &stAddr, sizeof(stAddr)) == -1)
    {
        return -2;
    }

    if(setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *) &iRcvBufLen, sizeof(iRcvBufLen)) == -1)
    {
    }
    return s;
}
