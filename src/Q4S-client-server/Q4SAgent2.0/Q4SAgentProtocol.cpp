#include "Q4SAgentProtocol.h"

#include <stdio.h>
#include <vector>
#include <sstream>


#include "..\Q4SCommon\ETime.h"
#include "..\Q4SCommon\Q4SMathUtils.h"
#include "Q4SAgentConfigFile.h"
#include "Q4SAgentFunctions.h"


#define     DEFAULT_CONN_ID     1

Q4SAgentProtocol::Q4SAgentProtocol ()
{
    clear();
}

Q4SAgentProtocol::~Q4SAgentProtocol ()
{
    done();
}



bool Q4SAgentProtocol::init()
{
    // Prevention done call
    done();

    bool ok = true;

    if (ok)
    {
        ok &= mAgentSocket.init();
    }

    if (ok)
    {
        ok &= mAgentSocket.startActionSender();
    }

    if (ok)
    {
        ok &= mReceivedMessages.init( );
    }

    if (ok)
    {
        ok &= openConnectionListening();
    }
    
    return ok;
}

void Q4SAgentProtocol::done()
{
    closeConnectionListening();
    mReceivedMessages.done( );
}

void Q4SAgentProtocol::clear()
{
}

bool Q4SAgentProtocol::Startlisten(){
	bool ok = true;
	ok=openListen();
	return ok;
}


bool Q4SAgentProtocol::openConnectionListening()
{
    bool ok = true;

    marrthrListenHandle = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
    
    return ok;
}

bool Q4SAgentProtocol::openListen() // hacer close
{
    bool ok = true;

    marrthrListenHandle2 = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )listenFn, ( void* ) this, 0, 0 );
    
    return ok;
}


void Q4SAgentProtocol::closeConnectionListening()
{
    bool ok = true;

    if( ok )
    {
        WaitForSingleObject( marrthrListenHandle, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}

void Q4SAgentProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        ok &= mAgentSocket.closeConnection( SOCK_DGRAM );
        WaitForSingleObject( marrthrDataHandle, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}


DWORD WINAPI Q4SAgentProtocol::listenFn( LPVOID lpData )
{
    Q4SAgentProtocol* q4sCP = ( Q4SAgentProtocol* )lpData;
    return q4sCP->listen( );
}


// State managing functions.

bool Q4SAgentProtocol::listen()
{
    printf("METHOD: listen at port %s\n", q4SAgentConfigFile.listenUDPPort.c_str());
    std::string message;
	 mAgentSocket.sendActionData("SET_BITRATE 5000");
    bool ok = true;
    
    if (q4SAgentConfigFile.demoMode)
    {
        mAgentSocket.sendActionData("SET_BITRATE 5000");
        Sleep (5000);
        mAgentSocket.sendActionData("SET_BITRATE 4000");
        Sleep (5000);
        mAgentSocket.sendActionData("SET_BITRATE 3000");
        Sleep (5000);
        mAgentSocket.sendActionData("SET_BITRATE 2000");
        Sleep (5000);
        mAgentSocket.sendActionData("SET_BITRATE 1000");
        
        ok = true;
    }
    if ( ok ) 
    {
        mReceivedMessages.readFirst( message ); 
    }
    return ok;
}


void Q4SAgentProtocol::end()
{
    closeConnections();
}

// Received data managing functions.

DWORD WINAPI Q4SAgentProtocol::manageUdpReceivedDataFn( LPVOID lpData )
{
    Q4SAgentProtocol* q4sCP = ( Q4SAgentProtocol* )lpData;
    return q4sCP->manageUdpReceivedData( );
}

bool Q4SAgentProtocol::manageUdpReceivedData( )
{
    bool                ok = true;
    char                udpBuffer[ 65536 ];
    unsigned long       bitRate = q4SAgentConfigFile.initialBitRate;
	float				Latency, Jitter;
	unsigned int		Packetloss;
	std::string			udpBufferString, StrLatency, StrJitter, StrPacketloss, TypeAlert, action;
	Actuator			actuator;


    mAgentSocket.startUdpListening( );
    while ( ok )
    {
        ok &= mAgentSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ));

        if( ok )
        {

			std::stringstream ss(udpBuffer);
			ss >> TypeAlert >> StrLatency >> Latency >> StrJitter >> Jitter >> StrPacketloss >> Packetloss ;
			/*printf(" Latency=> %f \n", Latency);
			printf(" Jitter=> %f \n", Jitter);
			printf(" TypeAlert=> %s \n", TypeAlert);*/



			// El mensaje de entrada debe ser siempre <ALERT Latency: xxx Jitter: xxxxxxx>
			// ALERT Latency: xxxx
			//	ALERT Jitter: xxxx
            // <ALERT Latency: 121.5 Jitter: 4.89473>
			actuator.ReadConfigFile();
			if (TypeAlert.compare("ALERT") == 0){
				cout << "Rx Alert " << endl;
				actuator.PathAlert(Jitter, Latency, Packetloss, action, TypeAlert);
				printf("Action ALERT=> %s \n", action.c_str());
				if(q4SAgentConfigFile.demoConnSocket){
					mAgentSocket.sendActionData(action.c_str());
				}
                else{
					system(action.c_str());
				}
			}
			else if (TypeAlert.compare("RECOVERY") == 0){
				cout << "Rx Recovery " << endl;
				actuator.PathRecovery(action, TypeAlert);
				printf("Action RECOVERY=> %s \n", action.c_str());
				if(q4SAgentConfigFile.demoConnSocket){
					mAgentSocket.sendActionData(action.c_str());
				}
				else{
					system(action.c_str());
				}
			}
			else {
				printf("Wrong Type Alert \n");
			}

        }
    }

    return ok;
}