
// deprecated

///*
// * testmain.cpp
// *
// *  Created on: Mar 17, 2010
// *      Author: darko
// */

//#include "afAvahiClient.h"
//#include "afServiceBrowser.h"
//#include "afLocalService.h"
//#include <string>
//#include <iostream>
//#include <sigc++/sigc++.h>
//#include <csignal>
//#include <stdlib.h>
//#include <cassert>
//#include <avahi-common/strlst.h>

//using namespace std;

//afAvahiClient avahiclient;

//void handleSIGINT(int sig) {
//	cout << ".CAUGHT SIG " << sig << endl;
//	avahiclient.stop();
//}

//void testCallback (afServiceBrowser* browser,
//    AvahiBrowserEvent event,
//	afRemoteService *service,
//	void* data) {
//	cout << "testCallback" << endl;
//}

//void testAdded (afRemoteService* rms) {
//	cout << " - TESTING SIGNAL: ADDEDD SERVICE: " << rms->getName() << endl;
//}

//int main(int argc, char* argv[]) {

////	assert(avahiclient.getAvahiClient());
//	signal(SIGTERM, handleSIGINT);
//	signal(SIGINT, handleSIGINT);
//	afServiceBrowser sbrowser(&avahiclient, "_rimres._tcp");
//	sbrowser.afServiceAdded.connect(sigc::ptr_fun(testAdded));
////	sbrowser.afServiceBrowserSignal.connect(sigc::ptr_fun(testCallback));

//	list<string> txt;
//	txt.push_back("darko=makreshanski");
//	txt.push_back("dd=makreshanski");
//	afLocalService service(&avahiclient, "darkoaftest", "_rimres._tcp", 12000, txt);

//	avahiclient.dispatch();

//}
