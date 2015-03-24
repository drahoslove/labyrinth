#include <iostream>
#include <boost/asio.hpp>
#include "components.h"

using namespace std;

/**
 * Trida pro zpracovani chyb
 */
class Err {
private:
	/**
	 * Chybove hlasky
	 */
	string ERR_MSG[2] = {
		"",
		"invalid port",
	};

public:
	/**
	 * Navratov kody (index do ERR_MSG)
	 */
	enum ecode {
		OK,
		INVALID_PORT,
	};

	/**
	 * Vypise chybovou hlasku na stderr a ukonci program
	 * @param  ecode Navratov kod
	 * @return       vraci Navratov kod
	 */
	ecode stop(ecode ecode){
		cerr << ERR_MSG[ecode] << endl;
		exit (ecode);
	}

	/**
	 * Vypise chybovou hlasku na stderr
	 * @param  ecode Navratov kod programu
	 * @return       Navratov kod programu
	 */
	ecode print(ecode ecode){
		cerr << ERR_MSG[ecode] << endl;
		return ecode;
	}
	
} err; // rovnou vytvorit globalni instanci tridy


/**
 * Vraci port zadany jako prvni argument programu
 * nebo nulu pokud je neco spatne
 * @param  argc 
 * @param  argv 
 * @return      port
 */
int getPort(int argc, char const *argv[]){
	int port = 0;
	if(argc > 1)
		port = atoi(argv[1]);
	return port;
}


/**
 * TOTO JE MAIN
 * @param  argc 
 * @param  argv 
 * @return      
 */
int main(int argc, char const *argv[])
{

	int port = getPort(argc, argv);
	if(port == 0){
		err.stop(Err::INVALID_PORT);
	}



	// herni plocha
	Board * board = new Board(5);
	std::vector<Item> items;
	for (Item i = 1; i <= 12; ++i){
		items.push_back(i);
	}

	board->placeItems(&items);


	// a jeden dilek navic
	Block * spareBlock = new Block(Shape::T, LEFT);
	spareBlock->rotate(3*LEFT);

	spareBlock->item = items[2];

	cout << board->toString() << endl;
	cout << spareBlock->toString() << endl;

	Coords pos1(1,2);
	Coords pos2(3,4);

	bool result = board->isConnected(pos1, pos2);
	cout << "Cesta je:" << result << endl;


	return 0;
}


// format save souboru
// 
// T0 -- 
// L3 I1 T0 I0 T0 L2 L2
// I0 L2 L1 T3 I1 I0 T0
// T1 I0 T0 L2 T1 L1 T3
// 


