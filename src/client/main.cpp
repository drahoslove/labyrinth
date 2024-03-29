/**
 * Projekt ICP 
 * Labyrint
 * 
 * Drahoslav Bednář - xbedna55
 * Jiří Kunčák -xkunca55
 * 
 * 2014/2015
 *
 * modul: main
 * vstupní bod konzolové verze klientského programu
 */

#include <iostream>
#include "client.h"

// #include <QtGui/QApplication>
// #include <QApplication>

// ///////////////////////////

using namespace std;

int main(int argc, char const *argv[])
{
	Address serveraddr;
	
	if(argc != 3){
		cerr << "Usage: client <host> <port>\n" << endl;
		return 1;
	}else{
		serveraddr.hostname = argv[1];
		serveraddr.port = argv[2];
	}


	boost::shared_ptr<boost::asio::io_service> io_service(new boost::asio::io_service());

	Client client(io_service.get());
	client.start(serveraddr);
	if(!client.isRunning()){
		std::cout << "nelze pripojit" << endl;
		return 1;
	}
	
	thread t(boost::bind(&boost::asio::io_service::run, io_service));
	string line;
	string cmd = "HI";
	string data = "";

	client.sendCommand(cmd, data);

	while(getline(cin, line) && client.isRunning()){
		split(line, ' ', &cmd, &data);

		if(cmd == "SUICIDE"){
			break;
		}
		if(cmd == "HELP"){
			string commands;
			commands = client.printCommands();
			cout << commands << endl;
			continue;
		}

		if(client.validCommand(cmd)){
			client.sendCommand(cmd, data);
		}else{
			cout << "Neplatny prikaz zadajte novy:" << endl;
		}

	}

	cout << "Program se ukoncuje" << endl;

	client.quit();
	//t.join(); //<--- to tu nikdy nemelo byt
	
	return 0;
}