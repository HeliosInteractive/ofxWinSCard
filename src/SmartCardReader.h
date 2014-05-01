#pragma once

#include "ofMain.h"
#include "WINSCARD.h"

class SmartCardReader
{
public :
	SmartCardReader() { } 
	~SmartCardReader( ) { } 

	void setup( string readerName ) ; 
	void update( ) ; 
	void draw( ) ; 

	void connect( ) ; 
	void listAllReaders( ) ; 
	void clearBuffers( ) ; 
	bool getStatus( ) ;
	string getCardUID( ) ; 
	int transmit( ) ; 

	string getSCardErrorMessage( int errorCode ) ; 
	void printHexString(CHAR* sPrefix, LPBYTE baData, DWORD dataLen);

	string readerLabel ; 
	LPCTSTR lReaderName ;
	SCARDCONTEXT			context ;
	SCARDHANDLE				card ;

	unsigned long			dwActProtocol;
	LPCBYTE					pbSend;
	DWORD					dwSend, dwRecv, size ; 
	LPBYTE					pbRecv;
	SCARD_IO_REQUEST		ioRequest;
	int						retCode;
    char					readerName [256];
	DWORD					SendLen, RecvLen, ByteRet;;
	BYTE					SendBuff[262], RecvBuff[262];
	SCARD_IO_REQUEST		IO_REQ;
	unsigned char			HByteArray[16];
};