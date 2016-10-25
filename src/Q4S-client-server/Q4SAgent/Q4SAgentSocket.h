#ifndef _Q4SAGENTSOCKET_H_
#define _Q4SAGENTSOCKET_H_

#include "Q4SSocket.h"

#include <stdio.h>
#include <list>

class Q4SAgentSocket
{
public:

    // Constructor-Destructor
    Q4SAgentSocket( );
    ~Q4SAgentSocket( );

    // Init-Done
    bool    init( );
    void    done( );

    bool    startUdpListening( );
    bool    stopWaiting( );
    bool    closeConnection( int socketType );
    bool    sendUdpData( int connectionId, const char* sendBuffer );
    bool    receiveUdpData( char* receiveBuffer, int receiveBufferSize, int& connectionId );

private:

    struct Q4SConnectionInfo 
    {
        int             id;
        sockaddr_in     peerUdpAddrInfo;
    };

    void    clear( );

    bool    initializeSockets( );
    bool    startListen( );
    bool    closeListenSocket( );
    bool    createUdpSocket( );
    bool    bindUdpSocket( );

    bool    getConnectionInfo( int connectionId, Q4SConnectionInfo*& pQ4SConnInfo );
    bool    getConnectionInfo( sockaddr_in& connectionInfo, Q4SConnectionInfo*& pQ4SConnInfo );

    SOCKET              mListenSocket;
    SOCKET              mUdpSocket;
    struct addrinfo*    mpAddrInfoResultUdp; 

    Q4SSocket           mq4sUdpSocket;

    std::list< Q4SConnectionInfo* >         listConnectionInfo;
};

#endif  // _Q4SAGENTSOCKET_H_