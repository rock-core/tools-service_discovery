
// deprecated

///*
// * testmain.cpp
// *
// *  Created on: Mar 17, 2010
// *      Author: darko
// */

//#include "AvahiClient.h"
//#include "ServiceBrowser.h"
//#include "LocalService.h"
//#include <string>
//#include <iostream>
//#include <sigc++/sigc++.h>
//#include <csignal>
//#include <stdlib.h>
//#include <cassert>
//#include <avahi-common/strlst.h>

//using namespace std;

//AvahiClient avahiclient;

//void handleSIGINT(int sig) {
//	cout << ".CAUGHT SIG " << sig << endl;
//	avahiclient.stop();
//}

//void testCallback (ServiceBrowser* browser,
//    AvahiBrowserEvent event,
//	RemoteService *service,
//	void* data) {
//	cout << "testCallback" << endl;
//}

//void testAdded (RemoteService* rms) {
//	cout << " - TESTING SIGNAL: ADDEDD SERVICE: " << rms->getName() << endl;
//}

//int main(int argc, char* argv[]) {

////	assert(avahiclient.getAvahiClient());
//	signal(SIGTERM, handleSIGINT);
//	signal(SIGINT, handleSIGINT);
//	ServiceBrowser sbrowser(&avahiclient, "_rimres._tcp");
//	sbrowser.ServiceAdded.connect(sigc::ptr_fun(testAdded));
////	sbrowser.ServiceBrowserSignal.connect(sigc::ptr_fun(testCallback));

//	list<string> txt;
//	txt.push_back("darko=makreshanski");
//	txt.push_back("dd=makreshanski");
//	LocalService service(&avahiclient, "darkotest", "_rimres._tcp", 12000, txt);

//	avahiclient.dispatch();

//}
