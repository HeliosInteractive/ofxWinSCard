#include "SmartCardReader.h"
#include <conio.h>
#include <stdio.h>

#define RcvLenMax 300l 
// Max. APDU Buffer length.
void SmartCardReader::setup( string _readerName ) 
{
	ofLogNotice( "Initializing SmartCard reader with name" ) << _readerName ; 
	readerLabel = _readerName ;
	int len = 64 ; 

	//Establish Context
	retCode = SCardEstablishContext( SCARD_SCOPE_USER,
									 NULL,
									 NULL,
									 &context );
	
	if( retCode != SCARD_S_SUCCESS )
	{
		ofLogError( "SmartCardReader::setup() FAILED TO ESTABLISH CONTEXT" ) << getSCardErrorMessage( retCode ) ;
		return ; 
	}
	else
	{
		ofLogNotice( "Context established successfully." ) ; 
	}

	listAllReaders( ) ; 

	connect( ) ; 
}

void SmartCardReader::listAllReaders( ) 
{

	LPTSTR          pmszReaders = NULL;
	LPTSTR          pReader;
	LONG            lReturn, lReturn2;
	DWORD           cch = SCARD_AUTOALLOCATE;

	// Retrieve the list the readers.
	// hSC was set by a previous call to SCardEstablishContext.
	lReturn = SCardListReaders( context,
							    NULL,
							    (LPTSTR)&pmszReaders,
							    &cch );

	if( retCode != SCARD_S_SUCCESS )
	{
		ofLogError( "SmartCardReader::setup() FAILED TO LIST ALL READERS" ) << getSCardErrorMessage( retCode ) ; 
		return;
	}
	else
	{
		// Do something with the multi string of readers.
        // Output the values.
        // A double-null terminates the list of values.
        pReader = pmszReaders;
        while ( '\0' != *pReader )
        {
            // Display the value.
            printf("Reader: %S\n", pReader );
            // Advance to the next value.
            pReader = pReader + wcslen((wchar_t *)pReader) + 1;
        }
        // Free the memory.
        lReturn2 = SCardFreeMemory( context ,
                                    pmszReaders );
        if ( SCARD_S_SUCCESS != lReturn2 )
            printf("Failed SCardFreeMemory\n");
	}

	if( readerName == NULL )
	{
	
		ofLogError( "READER NAME IS INVALID" ) << getSCardErrorMessage( retCode ) ;
		return;
	}
}

void SmartCardReader::update( ) 
{

}

void SmartCardReader::draw( ) 
{

}

	
void SmartCardReader::clearBuffers( ) 
{
	int index;
	
	for( index = 0; index <= 262; index++ )
	{
		SendBuff[index] = 0x00;
		RecvBuff[index] = 0x00;
	}
}


void SmartCardReader::connect( ) 
{
	DWORD Protocol = 1;
	char buffer1[100];
	char buffer2[100];

	const char * tempstr = readerLabel.c_str()  ; 

	char * cReaderName = new char[ 10 + 1 ];   
	strcpy( cReaderName , tempstr ) ; 

	//Connect to selected reader
	retCode = SCardConnectA( context,
							cReaderName  ,
							SCARD_SHARE_SHARED,
							SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1,
							&card,
							&dwActProtocol );

	if( retCode != SCARD_S_SUCCESS )
	{
	
		//Failed to connect to reader
		ofLogError ( "SmartCardReader::connect() SMART CARD CONNECT FAILURE" ) << getSCardErrorMessage( retCode ) ; 
		//DisplayOut( GetScardErrMsg( retCode ), RED );
		return;
		
	}
	else
	{
		ofLogNotice( " SmartCardReader::connect() Connection to " + readerLabel + " successful." ) ; 
	}

	//Successful connection to reader
//	IO_REQ.dwProtocol = Protocol;
//	IO_REQ.cbPciLength = sizeof( SCARD_IO_REQUEST );

	//cbReader.GetLBText( cbReader.GetCurSel(), buffer2 );
	//sprintf( buffer1, "%s %s \n", "Successful connection to ", buffer2 );
}

string SmartCardReader::getStatus ( ) 
{
	int index;
	char tempstr[262];
	char tempstr2[262];

	clearBuffers();
	SendBuff[0] = 0xFF;
	SendBuff[1] = 0x00;
	SendBuff[2] = 0x00;
	SendBuff[3] = 0x00;
	SendBuff[4] = 0x02;
	SendBuff[5] = 0xD4;
	SendBuff[6] = 0x04;

	SendLen = 7;
	RecvLen = 12;

	retCode = transmit();
	if(retCode != SCARD_S_SUCCESS)
	{
		ofLogError( "TRANSMIT failure!" ) ; 
		return "TRANSMIT FAILED" ; 
		return false ; 
	}
	else
	{
		ofLogNotice( "transmit() was a success!" ) ; 
	}

	//Interpret data
	sprintf( tempstr, "" );

	for(index = 0; index < RecvLen; index++)
	{
	
	//	sprintf( tempstr, "%s%02X", tempstr, RecvBuff[index] );
	
	}

	if( (strcmp( tempstr, "D505280000809000" ) == 0) || (strcmp( tempstr, "D505000000809000") == 0))
	{
	
		//no tag is in the field
		ofLogNotice( "> No tag is in the field: %02X" ) ; //, RecvBuff[1] );
		return "No tag is in the field" ; 
		//ofLogNotice( tempstr2 ) ; 
		//DisplayOut( tempstr2, BLACK ); 
	
	}
	else
	{
	
		//error code
		sprintf( tempstr2, "> Error Code: %02X", RecvBuff[2] );

		//DisplayOut( tempstr2, BLACK );
	
		//Field indicates if an external RF field is present and detected
        //(Field=0x01 or not (Field 0x00)
		if( RecvBuff[3] == 0x01 )
		{
		
			ofLogNotice( "> External RF field is Present and detected: %02X" ) ; //, RecvBuff[3]);
			return "External RF field is Present and detected" ; 
			//DisplayOut( tempstr2, BLACK );
		
		}
		else
		{
			
			ofLogNotice( "> External RF field is NOT Present and NOT detected: %02X" ) ; //, RecvBuff[3]);
			return "External RF field is NOT Present and NOT detected" ; 
			//DisplayOut( tempstr2, BLACK );
		
		}

		//Number of targets currently controlled by the PN532 acting as initiator.The default value is 1
		ofLogNotice( "> Number of Target: %02X" ) << RecvBuff[4] ;
		//DisplayOut( tempstr2, BLACK );

		//Logical number
		ofLogNotice( "> Number of Target: %02X" ) << RecvBuff[5] ;
		//DisplayOut( tempstr2, BLACK );

		//Bit rate in reception
		switch( RecvBuff[6] )
		{
		
			case 0x00: 
				ofLogNotice( "> Bit rate in reception: %02X (106 kbps)" ) ; //, RecvBuff[6]);
				break;

			case 0x01: 
				ofLogNotice( "> Bit rate in reception: %02X (212 kbps)" ) ; //, RecvBuff[6]);
			   break;

			case 0x02: 
				ofLogNotice( "> Bit rate in reception: %02X (424 kbps)" ) ; //, RecvBuff[6]);
				break;
		
		}

		//Bit rate in transmission
		switch( RecvBuff[7] )
		{
		
			case 0x00: ofLogNotice( "> Bit rate in transmission: %02X (106 kbps)" ) ;//, RecvBuff[7]);
					  // DisplayOut( tempstr2, BLACK );
					   break;

			case 0x01: ofLogNotice( "> Bit rate in transmission: %02X (212 kbps)" ) ;//, RecvBuff[7]);
					  // DisplayOut( tempstr2, BLACK );
					   break;

			case 0x02: ofLogNotice( "> Bit rate in transmission: %02X (424 kbps)" ) ; //, RecvBuff[7]);
					 //  DisplayOut( tempstr2, BLACK );
					   break;
		
		}

		switch( RecvBuff[8] )
		{
		
			case 0x00: ofLogNotice( "> Modulation type: %02X (ISO14443 or MiFare)" ) ; //, RecvBuff[8] );
					 //  DisplayOut( tempstr2, BLACK );
					   break;

			case 0x01: ofLogNotice( "> Modulation type: %02X (Active Mode)" ) ; //, RecvBuff[8] );
					//  DisplayOut( tempstr2, BLACK );
					   break;

			case 0x02:ofLogNotice( tempstr2, "> Modulation type: %02X (Innovision Jewel Tag)" ) ; //, RecvBuff[8] );
					//   DisplayOut( tempstr2, BLACK );
					   break;

			case 0x10: ofLogNotice( "> Modulation type: %02X (Felica)" ) ; //, RecvBuff[8] );
					//   DisplayOut( tempstr2, BLACK );
					   break;
		
		}
	}
	return "all good" ; 
}

int SmartCardReader::transmit( ) 
{
	char tempstr[262];
	char tempstr2[262];
	int index;

	sprintf( tempstr, ">" );
	for( index = 0; index <= SendLen-1  ; index++ )
	{

		sprintf( tempstr, "%s %02X", tempstr, SendBuff[index] );
	
	}

	sprintf( tempstr, "%s\n", tempstr );

	//DisplayOut( tempstr, BLACK );
		
	retCode = SCardTransmit(card,
							NULL,
							SendBuff,
							SendLen,
							NULL,  
							RecvBuff,
							&RecvLen);

	cout << "buff : " << endl << RecvBuff << endl ; 
	cout << "vlen" << endl << RecvLen << endl ; 
	if( retCode != SCARD_S_SUCCESS )
	{
		ofLogError( " SCardTransmit Error " ) << getSCardErrorMessage( retCode ) ; 
		//DisplayOut( GetScardErrMsg( retCode ), RED );
		
	
	}
	else
	{
		ofLogNotice( "transmit was a success!" ) ; 
		sprintf( tempstr2, ">" );
		for( index = 0; index <= RecvLen - 1; index++ )
		{
		
			sprintf( tempstr2, "%s %02X",tempstr2, RecvBuff[index] );
		
		}
		sprintf( tempstr2, "%s \n", tempstr2 );
		cout << tempstr2 << endl ; 


		//DisplayOut( tempstr2, BLACK );


	}

	return retCode;
}

//LONG PCSC_Exchange(LPCBYTE pbSendBuffer ,DWORD  cbSendLength ,LPBYTE  pbRecvBuffer ,LPDWORD pcbRecvLength )


string SmartCardReader::getCardUID ( )
{
	clearBuffers() ; 
	 // Get Data: CLA = 0xFF, INS = 0xCA, P1 = 0x00, P2 = 0x00, Le = 0x00
    BYTE baCmdApduGetData[] = { 0xFF, 0xCA, 0x00, 0x00, 0x00};
	BYTE baResponseApdu[300];    
    DWORD lResponseApduLen = 0;

	LPCBYTE pbSendBuffer = baCmdApduGetData ; 
	DWORD cbSendLength = (DWORD)sizeof(baCmdApduGetData) ; 

	LPBYTE  pbRecvBuffer = baResponseApdu ; 
	LPDWORD pcbRecvLength  = &lResponseApduLen ; 

    BYTE atr[40];
    INT     atrLength;
    LONG lRetValue;

	LPCSCARD_IO_REQUEST  ioRequest;

	ioRequest = SCARD_PCI_T1;

	/*
	switch (m_dwActiveProtocol)
	{*/
		//case SCARD_PROTOCOL_T0:
			//ioRequest = SCARD_PCI_T0;
			//break;
			/*
		case SCARD_PROTOCOL_T1:
						break;

		default:
			ioRequest = SCARD_PCI_RAW;
			break;
	}*/
	
	*pcbRecvLength = RcvLenMax;

    // APDU exchange.
	lRetValue = SCardTransmit( card,		// Card handle.
							ioRequest,		// Pointer to the send protocol header.
							pbSendBuffer,	// Send buffer.
							cbSendLength,	// Send buffer length.
							NULL,			// Pointer to the rec. protocol header.
							pbRecvBuffer,	// Receive buffer.
							pcbRecvLength);	// Receive buffer length.
	
	//PCSC_STATUS(lRetValue,"SCardTransmit");	
	
	
	//printHexString( "\n   --> C-Apdu: 0x",(LPBYTE)pbSendBuffer, cbSendLength ) ;	
	//printHexString(  "   <-- R-Apdu: 0x",pbRecvBuffer, *pcbRecvLength );
	//printf("       SW1SW2: 0x%02X%02X\n\n",pbRecvBuffer[*pcbRecvLength - 2], pbRecvBuffer[*pcbRecvLength - 1]); 
	

	/* Found on Stack Overflow 
	http://stackoverflow.com/questions/7639656/getting-a-buffer-into-a-stringstream-in-hex-representation
	*/

	stringstream toHex ;
	toHex << std::hex << std::setfill('0') ; 
	for( int i = 0; i < lResponseApduLen-2 ; i++)		
	{
		toHex << std::setw(2) << static_cast<unsigned>( baResponseApdu[i] ) ;	
	}

	 if( baResponseApdu[lResponseApduLen - 2] == 0x90 &&
        baResponseApdu[lResponseApduLen - 1] == 0x00)
    {
       // Contactless card detected.
        // Retrieve the card serical number (UID) form the response APDU.
        //printHexString("Card Serial Number (UID): 0x", baResponseApdu, lResponseApduLen - 2);

		/*
        if( getClessCardType(atr) == Mifare1K)
        {
            printf("Card Type: MIFARE Classic 1k");
        }
        else if( getClessCardType(atr) == Mifare4K)
        {
            printf("Card Type: MIFARE Classic 4k");
        }
        else if( getClessCardType(atr) == MifareUL)
        {
            printf("Card Type: MIFARE Ultralight");
        }*/
		return toHex.str() ;
    }
	 else
	 {
		 ofLogError( "Contactless card not detected !" ) ;
		 return "" ;
	 }
}

void SmartCardReader::printHexString(CHAR* sPrefix, LPBYTE baData, DWORD dataLen)
{
	DWORD i;

	printf("%s", sPrefix);

	for(i = 0; i < dataLen;i++)		
	{
		printf("%02X ",baData[i]);
	}
	
	printf("\n");
}


string SmartCardReader::getSCardErrorMessage( int errorCode ) 
{
	switch( errorCode )
	{
		// Smartcard Reader interface errors
		case SCARD_E_CANCELLED:
			return ("The action was canceled by an SCardCancel request.\n");
			break;
		case SCARD_E_CANT_DISPOSE:
			return ("The system could not dispose of the media in the requested manner.\n");
			break;
		case SCARD_E_CARD_UNSUPPORTED:
			return ("The smart card does not meet minimal requirements for support.\n");
			break;
		case SCARD_E_DUPLICATE_READER:
			return ("The reader driver didn't produce a unique reader name.\n");
			break;
		case SCARD_E_INSUFFICIENT_BUFFER:
			return ("The data buffer for returned data is too small for the returned data.\n");
			break;
		case SCARD_E_INVALID_ATR:
			return ("An ATR string obtained from the registry is not a valid ATR string.\n");
			break;
		case SCARD_E_INVALID_HANDLE:
			return ("The supplied handle was invalid.\n");
			break;
		case SCARD_E_INVALID_PARAMETER:
			return ("One or more of the supplied parameters could not be properly interpreted.\n");
			break;
		case SCARD_E_INVALID_TARGET:
			return ("Registry startup information is missing or invalid.\n");
			break;
		case SCARD_E_INVALID_VALUE:
			return ("One or more of the supplied parameter values could not be properly interpreted.\n");
			break;
		case SCARD_E_NOT_READY:
			return ("The reader or card is not ready to accept commands.\n");
			break;
		case SCARD_E_NOT_TRANSACTED:
			return ("An attempt was made to end a non-existent transaction.\n");
			break;
		case SCARD_E_NO_MEMORY:
			return ("Not enough memory available to complete this command.\n");
			break;
		case SCARD_E_NO_SERVICE:
			return ("The smart card resource manager is not running.\n");
			break;
		case SCARD_E_NO_SMARTCARD:
			return ("The operation requires a smart card, but no smart card is currently in the device.\n");
			break;
		case SCARD_E_PCI_TOO_SMALL:
			return ("The PCI receive buffer was too small.\n");
			break;
		case SCARD_E_PROTO_MISMATCH:
			return ("The requested protocols are incompatible with the protocol currently in use with the card.\n");
			break;
		case SCARD_E_READER_UNAVAILABLE:
			return ("The specified reader is not currently available for use.\n");
			break;
		case SCARD_E_READER_UNSUPPORTED:
			return ("The reader driver does not meet minimal requirements for support.\n");
			break;
		case SCARD_E_SERVICE_STOPPED:
			return ("The smart card resource manager has shut down.\n");
			break;
		case SCARD_E_SHARING_VIOLATION:
			return ("The smart card cannot be accessed because of other outstanding connections.\n");
			break;
		case SCARD_E_SYSTEM_CANCELLED:
			return ("The action was canceled by the system, presumably to log off or shut down.\n");
			break;
		case SCARD_E_TIMEOUT:
			return ("The user-specified timeout value has expired.\n");
			break;
		case SCARD_E_UNKNOWN_CARD:
			return ("The specified smart card name is not recognized.\n");
			break;
		case SCARD_E_UNKNOWN_READER:
			return ("The specified reader name is not recognized.\n");
			break;
		case SCARD_F_COMM_ERROR:
			return ("An internal communications error has been detected.\n");
			break;
		case SCARD_F_INTERNAL_ERROR:
			return ("An internal consistency check failed.\n");
			break;
		case SCARD_F_UNKNOWN_ERROR:
			return ("An internal error has been detected, but the source is unknown.\n");
			break;
		case SCARD_F_WAITED_TOO_LONG:
			return ("An internal consistency timer has expired.\n");
			break;
		case SCARD_W_REMOVED_CARD:
			return ("The smart card has been removed and no further communication is possible.\n");
			break;
		case SCARD_W_RESET_CARD:
			return ("The smart card has been reset, so any shared state information is invalid.\n");
			break;
		case SCARD_W_UNPOWERED_CARD:
			return ("Power has been removed from the smart card and no further communication is possible.\n");
			break;
		case SCARD_W_UNRESPONSIVE_CARD:
			return ("The smart card is not responding to a reset.\n");
			break;
		case SCARD_W_UNSUPPORTED_CARD:
			return ("The reader cannot communicate with the card due to ATR string configuration conflicts.\n");
			break;
		}
		return ("Error is not documented.\n");
}