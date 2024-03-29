/**
 * Projekt ICP 
 * Labyrint
 * 
 * Drahoslav Bednář - xbedna55
 * Jiří Kunčák -xkunca55
 * 
 * 2014/2015
 *
 * modul: client
 * třída: Client
 * 	řeší zpracování vstupu od uživatele a komunikaci ze serverem
 */

#include "client.h"

Client::Client(boost::asio::io_service* io_service){
	Client::io_service = io_service;
}

void Client::start(Address serveraddr){	
	boost::system::error_code ec;
	boost::asio::ip::tcp::resolver resolver(*io_service);
	boost::asio::ip::tcp::resolver::query query(serveraddr.hostname, serveraddr.port);
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, ec);
	if(ec){
		running = false;
		return;
	}
	connection = new Connection(*io_service);

	readMsg = "";

	boost::asio::connect(connection->socket, endpoint_iterator, ec);
	if(ec){
		running = false;
		return;
	}
	connection->recv_async(&readMsg, boost::bind(&Client::handleRead, this));

	state = NONE;
	running = true;
};

Client::~Client(){
	delete connection;
}

bool Client::isRunning(){
	return running;
}

void Client::quit(){
	running = false;
}

void Client::handleConnect(const boost::system::error_code& error){
	if (!error){
		connection->recv_async(&readMsg, boost::bind(&Client::handleRead, this));
	}
}

void Client::sendCommand(std::string command, std::string data=""){
	sendedCmds.push_back(command);
	io_service->post(boost::bind(&Client::doSendCommand, this, command+" "+data));
}

void Client::doSendCommand(std::string command){
	connection->send(&command);
}

void Client::handleRead(){
	if(!isRunning()){
		return;
	}

	// std::cout << "msg:" << readMsg << endl;

	string rcvCmd, rcvData, msg, cmd;
	split(readMsg, ' ', &rcvCmd, &rcvData);
	if(rcvCmd != "OK" && rcvCmd != "NOPE"){
		msg = doActionServer(rcvCmd, rcvData);
	}else{
		cmd = sendedCmds.front();
		msg = doActionClient(cmd, rcvCmd, rcvData);
		sendedCmds.pop_front();
	}

	if(msg.size()){
		cout << msg << endl;
	}

	if(isRunning()){
		readMsg = "";
		connection->recv_async(&readMsg, boost::bind(&Client::handleRead, this));
	}
}

string Client::doActionServer(string recvCmd, string data){
	string msg;

	//cout << "prikaz od serveru: --->" << recvCmd << "<---" << endl;

	// all states
	if (recvCmd == "DIE"){
		running = false;
		return "Server se nastval!";
	}
	if (recvCmd == "POKE"){
		return "Server te stouchnul!";
	}
	if (recvCmd == "SHOOT"){
		running = false;
		return "Byl jsi sestrelen bozi rukou parchante!";
	}

	// WAITING
	if (recvCmd == "INVITATION"){
		state = INVITED;
		return "Byl jsi pozvan do hry hracem " + data + ". Prijimas vyzvu?";
	}

	// READY or PLAYING
	if (recvCmd == "GAMECANCELED"){
		figures.clear();
		scoreboard.clear();
		delete board;
		state = WAITING;
		return "Hra byla ukoncena. Pripojte se do jine nebo zaloz svoji.";
	}
	if (recvCmd == "INIT"){
		state = PLAYING;
		string scboarddata, boardformat;
		split(data, ' ', &scboarddata, &boardformat);
		board = new Board(boardformat);
		initScoreboard(scboarddata);
		cout << "Hra zacala." << endl;
		printGamedesk();
		return "";
	}
	if (recvCmd == "YOURTURN"){
		state = ONTURN;
		return "Jsi na tahu.";
	}

		// commands for board change
	if (recvCmd == "ROTATED"){
		this->doRotate();
		printGamedesk();
		return "";
	}
	if (recvCmd == "SHIFTED"){
		this->doShift(data);
		printGamedesk();
		return "";
	}
	if (recvCmd == "MOVED"){
		this->doMove(data);
		printGamedesk();
		return "";
	}
	if (recvCmd == "SAVED"){
		return "Hra byla ulozena.";
	}
	if (recvCmd == "ENDGAME"){
		state = WAITING;
		figures.clear();
		scoreboard.clear();
		delete board;
		return "Konec hry. Vyhral hrac --->" + data + "<--- HOORAY!";
	}

	// CREATING
	if (recvCmd == "READYLIST"){
		return "Seznam hracu ve vasi hre:\n" + formatPlayers(data);
	}
	
	running = false;
	return "Server se zblaznil.";
}


string Client::doActionClient(string cmd, string response, string data){
	string msg = "";

	switch(state){
		case NONE:
			if(cmd == "HI" && response == "OK"){
				state = STARTED;
				msg = "Zadejte svuj nick:";
			}
			break;
		case STARTED:
			if(cmd == "IAM"){
				if(response == "OK"){
					state = WAITING;				
					msg = "Nick je v poradku. Pokracuj.";
				}else{
					msg = "Nick je spatny. Zadejte novy:";					
				}
			}
			if(cmd == "GODMODE" && response == "OK"){
				msg = "A ted jsi buh";
				state = GODMODE;
			}
			break;
		case WAITING:		
			if(cmd == "WHOISTHERE" && response == "OK"){
				msg = formatPlayers(data);
			}
			
			if(cmd == "CREATE" && response == "OK"){
				state = INVITING;
				msg = "Nyni muzes pozvat hrace";
			}
			break;
		case INVITED:
			if(cmd == "ACCEPT"){
				if(response == "OK"){
					state = READY;
					msg = "Prijmul si hru. Cekej nez hra zacne.";
				}else{
					state = WAITING;
					msg = "Hra je uz bohuzel plna. Pockej nez te nekdo pozve nebo zaloz hru.";
				}
			}
			if(cmd == "DECLINE" && response == "OK"){
				state = WAITING;
				msg = "Hru jsi neprijmul. Pockej nez te nekdo pozve nebo zaloz hru.";
			}
			break;
		case READY:
			break;
		case INVITING:
			if(cmd == "INVITE"){
				if(response == "OK"){
					msg = "Hrac " + data + " byl pozvan";
				}
				if(response == "NOPE"){
					msg = "Hrac " + data + " nemohl byt pozvan";
				}
			}
			if(cmd == "WHOISTHERE" && response == "OK"){
				msg = formatPlayers(data);
			}
			if(cmd == "NEWGAME"){
				if(response == "OK"){
					state = CREATING_NEW;
					msg = "Vyber nastaveni nove hry.";
				}else{
					msg = "Nemas dostatek hracu.";
				}
			}
			if(cmd == "LOADGAME"){
				if(response == "OK"){
					state = CREATING_LOAD;
					msg = "Vyber ulozenou hru.";
					msg += formatSavedGames(data);
				}else{
					msg = "Pro tento pocet hracu neni vhodna ulozena hra.";
				}
			}
			break;
		case CREATING_NEW:
			if(cmd == "NEW"){
				if(response == "OK"){
					state = READY;
					msg = "Cekej nez hra zacne.";
				}else{
					msg = "Zadal jsi spatne rozmery hry.";
				}
			}
			break;
		case CREATING_LOAD:
			if(cmd == "LOAD"){
				if(response == "OK"){
					state = READY;
					msg = "Cekej nez hra zacne.";
				}else{
					msg = "Hra nejde nacist";
				}
			}
			break;
		case PLAYING:
			break;
		case ONTURN:
			if(cmd == "ROTATE"){
				if(response == "OK"){
					msg = "Blok otocen";
				}else{
					msg = "Blok nemuze byt otocen protoze jsi uz shiftnul. Nyni pohni figurkou.";
				}
			}
			if(cmd == "SHIFT"){
				if(response == "OK"){
					if (data == "BLOCKED"){
						state = PLAYING;
						msg = "Jsi zablokovan. Na tahu je dalsi hrac.";
					}else{
						msg = "Shift byl proveden.";
					}
				}else{
					if (data == "COORDS"){
						msg = "Zadal jsi spatne souradnice pro shift.";
					}else{
						msg = "Shift jsi uz jednou udelal. Nyni tahni figurkou.";	
					}
				}
			}
			if(cmd == "MOVE"){
				if(response == "OK"){
					if(state == ONTURN){
						state = PLAYING;
						msg = "Na tahu je dalsi hrac";	
					}else{
						msg = "";
					}
				}else{
					msg = "Tento tah je neproveditelny. Tahni znovu.";
				}
			}
			if(cmd == "SAVE"){
				if(response == "OK"){
					// nic - info o tom, ze hra byla ulozena se zpracovava v action od serveru
				}else{
					msg = "Hra nemuze byt ulozena pod timto nazvem";
				}
			}
			break;	
		case GODMODE:
			if(cmd == "IAM"){
				if(response == "OK"){
					msg = "Nick je v poradku. Pokracuj.";
				}else{
					msg = "Nick je spatny. Zadejte novy:";					
				}
			}
			if(cmd == "GODMODE" && response == "OK"){
				msg = "Uz jednou buh jsi tak neser!";
			}
			if(cmd == "WHOISTHERE" && response == "OK"){
				msg = formatPlayers(data);
			}
			if(cmd == "KILL" && response == "OK"){
				msg = "Mas dalsi zarez na pazbe";
			}
			break;
	}

	return msg;
}

bool Client::validCommand(string cmd){
	switch(state){
		case STARTED:
			if (cmd == "IAM"){
				return true;
			}
			if (cmd == "GODMODE"){
				return true;
			}
			break;
		case WAITING:		
			if(cmd == "WHOISTHERE"){
				return true;
			}
			if(cmd == "CREATE"){
				return true;
			}
			break;
		case INVITED:
			if(cmd == "ACCEPT"){
				return true;
			}
			if(cmd == "DECLINE"){
				return true;
			}
			break;
		case READY:
			break;
		case INVITING:
			if(cmd == "INVITE"){
				return true;
			}
			if(cmd == "WHOISTHERE"){
				return true;
			}
			if(cmd == "NEWGAME"){
				return true;
			}
			if(cmd == "LOADGAME"){
				return true;
			}
			break;
		case CREATING_NEW:
			if(cmd == "NEW"){
				return true;
			}
			break;
		case CREATING_LOAD:
			if(cmd == "LOAD"){
				return true;
			}
			break;	
		case PLAYING:
			break;
		case ONTURN:
			if(cmd == "ROTATE"){
				return true;
			}
			if(cmd == "SHIFT"){
				return true;
			}
			if(cmd == "MOVE"){
				return true;
			}
			if(cmd == "SAVE"){
				return true;
			}
			break;
		case GODMODE:
			if (cmd == "IAM"){
				return true;
			}
			if(cmd == "WHOISTHERE"){
				return true;
			}
			if(cmd == "GODMODE"){
				return true;
			}
			if(cmd == "KILL"){
				return true;
			}
			break;
	}

	return false;
} 

string Client::formatPlayers(string data){
	string first, msg, position;
	msg = "Zacatek vypisu\n";
	int pos = 1;
	while(data != ""){
		split(data, ' ', &first, &data);
		msg += "\t" + itos(pos) + ". " + first + "\n";
		pos++;
	}
	msg += "Konec vypisu";

	return msg;
}

string Client::formatSavedGames(string data){
	string first, msg, position;
	msg = "Ulozene hry: \n";
	while(data != ""){
		split(data, ' ', &first, &data);
		msg += "\t" + first + "\n";
	}
	msg += "-----";

	return msg;
}

void Client::printGamedesk(){
	string color;
	cout << "SCOREBOARD" << endl;
	for(auto &line : scoreboard){
		cout << "  ";
		color = "";
		color += line.color;
		printColored(color);
		cout << " " + line.nickname + " " + itos(line.points) + " ";
		cout << line.card;
		cout << endl << endl;		
	}
	printColored(board->toString());
	cout << endl;
}

void Client::initFigure(char x, char y, char color){
	Color newcolor = ctocolor(color);
	Figure* figure = new Figure(newcolor);
	board->placeFigure(figure);
	figure->pos.x = x - 'A';
	figure->pos.y = y - 'A';
	figures.push_back(figure);
}

void Client::initScoreboard(string data){
	string player;
	char score;
	Scoreline line;
	while(data != ""){
		split(data, ';', &player, &data);
		line.color = player[0];
		player.erase(player.begin());
		initFigure(player[0], player[1], line.color);		
		player.erase(player.begin());
		player.erase(player.begin());
		line.card = player[0];
		player.erase(player.begin());		
		score = player[0];
		player.erase(player.begin());
		line.nickname = player;
		line.points = score - '0';
		scoreboard.push_back(line);
	}
}

void Client::doRotate(){
	board->rotate();
}

void Client::doShift(string data){
	char color = data[0];
	char foundcard = data[3];
	string coords = "";
	coords += data[1];
	coords += data[2];
	board->shift(coords);
	addPoint(foundcard, color);
}

void Client::doMove(string data){
	for(auto &figure : figures){
		char color = colortoc(figure->getColor());
		if(color == data[0]){
			Coords dest(data[1]-'A', data[2]-'A');
			figure->pos = dest;
			addPoint(data[3], color);
		}
	}
}

void Client::addPoint(char foundcard, char color){
	if(foundcard != '0'){
		for(auto &line : scoreboard){
			if(color == line.color){
				Item card;
				card = line.card - 'a' + 1;
				board->pickUpItem(card);
				line.points++;
				line.card = foundcard;
			}
		}
	}	
}

string Client::printCommands(){
	string msg = "Pouzitelne prikazy jsou:\n";

	switch(state){
		case STARTED:
			msg += "IAM [nickname]";
			break;
		case WAITING:
			msg += "WHOISTHERE\n";
			msg += "CREATE";
			break;
		case INVITED:
			msg += "ACCEPT\n";
			msg += "DECLINE";
			break;
		case READY:
			break;
		case INVITING:
			msg += "INVITE [nickname hrace]\n";
			msg += "WHOISTHERE\n";
			msg += "NEWGAME\n";
			msg += "LOADGAME";
			break;
		case CREATING_NEW:
			msg += "NEW [A-E pro vyber velikosti pole][0/1 pro pocet karet 12/24]";
			break;
		case CREATING_LOAD:
			msg += "LOAD [nazev hry]";
			break;	
		case PLAYING:
			break;
		case ONTURN:
			msg += "ROTATE\n";
			msg += "SHIFT [0-3 pro vyber strany - 0 nahore, 1 vpravo, ...][A-F pro vyber radku/sloupce oznaceneho sipkou - A nahore/vlevo]\n";
			msg += "MOVE [A-F - souradnice radku (A - vrchni)][A-F - souradnice sloupce (A - levy)]\n";
			msg += "SAVE [nazev hry]";
			break;
		case GODMODE:
			msg += "IAM [nick]\n";
			msg += "WHOISTHERE\n";
			msg += "GODMODE\n";
			msg += "KILL [nick]";
			break;
	}

	return msg;
} 