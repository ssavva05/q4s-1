#ifndef _Q4SSERVERSOCKET_H_
#define _Q4SSERVERSOCKET_H_

#include "Q4SSocket.h"

#include <stdio.h>
#include <list>

class Q4SServerSocket
{
public:

    // Constructor-Destructor
    Q4SServerSocket( );
    ~Q4SServerSocket( );

    // Init-Done
    bool    init( );
    void    done( );

    bool    startTcpListening( );
    bool    waitForTcpConnection( int connectionId );
    bool    waitForUdpConnections( );
    bool    stopWaiting( );
    bool    closeConnection( int socketType );
    bool    sendTcpData( int connectionId, const char* sendBuffer );
    bool    receiveTcpData( int connectionId, char* receiveBuffer, int receiveBufferSize );
    bool    sendUdpData( int connectionId, const char* sendBuffer );
    bool    receiveUdpData( char* receiveBuffer, int receiveBufferSize, int& connectionId );

private:

    struct Q4SConnectionInfo 
    {
        int             id;
        int             udpId;
        Q4SSocket       q4sTcpSocket;
        sockaddr_in     peerTcpAddrInfo;
        sockaddr_in     peerUdpAddrInfo;
    };

    void    clear( );

    bool    initializeSockets( );
    bool    createListenSocket( );
    bool    bindListenSocket( );
    bool    startListen( );
    bool    acceptClientConnection( Q4SConnectionInfo* connectionInfo );
    bool    closeListenSocket( );
    bool    createUdpSocket( );
    bool    bindUdpSocket( );

    bool    getTcpSocket( int connectionId, Q4SSocket*& pQ4SSocket );
    bool    getConnectionInfo( int connectionId, Q4SConnectionInfo*& pQ4SConnInfo );
    bool    getConnectionInfo( sockaddr_in& connectionInfo, Q4SConnectionInfo*& pQ4SConnInfo );

    SOCKET              mListenSocket;
    SOCKET              mUdpSocket;
    struct addrinfo*    mpAddrInfoResultTcp; 
    struct addrinfo*    mpAddrInfoResultUdp; 

    Q4SSocket           mq4sTcpSocket;
    Q4SSocket           mq4sUdpSocket;

    std::list< Q4SConnectionInfo* >         listConnectionInfo;
};

#endif  // _Q4SSERVERSOCKET_H_