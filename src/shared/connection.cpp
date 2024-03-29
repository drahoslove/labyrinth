/**
 * Projekt ICP 
 * Labyrint
 * 
 * Drahoslav Bednář - xbedna55
 * Jiří Kunčák -xkunca55
 * 
 * 2014/2015
 *
 * modul: connection
 * 	společné pro klienta i server
 * Třída:
 * řeší odesílání a čtení zpráv z/do socketu
 */

#include "connection.h"

using namespace std;

Connection::Connection(boost::asio::io_service & io_service): socket(io_service){

}

Connection::~Connection(){
	socket.close();
}

void Connection::recv(std::string * target){
	boost::asio::read_until(
		socket,
		rbuffer,
		'\n'
	);

	std::istream readStream(&rbuffer);
	std::getline(readStream, *target);
	rbuffer.consume(rbuffer.size());
	// std::cout << "Recv:" << *target << std::endl;
}

void Connection::send(std::string * message){
	// std::cout <<  "Send:" << * message << std::endl;
	std::string msg = *message + "\n";
	boost::asio::write(
		socket,
		boost::asio::buffer(msg),
		boost::asio::transfer_all()
	);
}


void Connection::send_async(std::string * message, boost::function<void()> handler )
{
	std::string msg = *message + "\n";
	boost::asio::async_write(
		socket,
		boost::asio::buffer(msg),
		boost::bind(handler)
	);
}


void Connection::recv_async(std::string * target,  boost::function<void()> handler )
{
	this->target = target;
	boost::asio::async_read_until(
		socket,
		rbuffer,
		'\n',
		boost::bind(&Connection::handle_recv, this, handler)
	);
}

void Connection::handle_recv(boost::function<void()> handler)
{

	std::istream is(&rbuffer);
	std::getline(is, *target, '\n');
	handler();
}

