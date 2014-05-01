#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	cardReader.setup( "ACS ACR122 0") ; 
	//cardReader.getStatus( ) ; 
	//cardReader.getCardUID() ; 
}

//--------------------------------------------------------------
void testApp::update(){
	cardReader.update( ) ;
}

//--------------------------------------------------------------
void testApp::draw(){
	stringstream ss ; 
	ss << "U - get card UID" << endl ; 
	ss << "E - export card UIDs read" << endl ; 
	ss << "scanned card history... " << endl; 
	for ( int c = 0 ; c < readCardIds.size() ; c++ ) 
	{
		ss <<"[" << c << "] " << readCardIds[c] << endl ; 
	}

	ofDrawBitmapStringHighlight( ss.str() , 15 , 15 ) ; 
}

void testApp::exportUIDs( )  
{
	ofFileDialogResult saveResult  = ofSystemSaveDialog( "NFC_ids.txt" , "select a filename to export UIDs" ) ; 
	if ( saveResult.bSuccess ) 
	{
		stringstream ss ; 
		for ( int i = 0 ; i < readCardIds.size() ; i++ ) 
		{
			ss << "[" << i << "] " << readCardIds[i] << endl ; 
		}
		ofBuffer exportBuffer( ss.str() ) ; 
		ofBufferToFile( saveResult.getPath() , exportBuffer , false ) ; 
	}
	else
	{
		ofLogError( "User did not complete the dialog box ! Nothing has exported." ) ; 
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	string cardResponse ; 
	switch ( key ) 
	{
		case 'u':
		case 'U':
			cardResponse = cardReader.getCardUID() ; 
			if ( cardResponse != "" ) 
			{
				readCardIds.push_back( cardResponse ) ; 
			}
			break ;

		case 'e':
		case 'E':
			exportUIDs( ) ; 
			break ; 
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
