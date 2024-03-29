/**
 * Projekt ICP 
 * Labyrint
 * 
 * Drahoslav Bednář - xbedna55
 * Jiří Kunčák -xkunca55
 * 
 * 2014/2015
 *
 * modul: components
 * 	společné pro klienta i server, reprezentuje jednotlivé komponenty hry
 *
 * Třídy a typy:
 * 	Figure
 * 		- figurka hráče s polohou a barvou
 * 	Board
 * 		- herní plochy bludiště obsahuje políčka a figurky
 * 	Block
 * 	 	- jedno políčko bludiště obsahhuje Item
 * 	Item
 * 		- představuje předmět nebo kartu
 * 	
 */

#include <ctime>
#include <boost/random.hpp>
#include "components.h"


std::time_t now = std::time(0);
boost::random::mt19937 gen{static_cast<std::uint32_t>(now)};

int randomInt(int max){
	return gen() % max; // <0;max)
}

Shape randomShape(){
	int i = randomInt(3);
	switch(i){
		case 0:
			return Shape::I;
			break;
		case 1:
			return Shape::L;
			break;
		case 2:
			return Shape::T;
			break;
	}
	return Shape::O;
}

/////////////////////////////////////////////////////////////
// metody tridy Pack:

void Pack::shuffle(){
	Item card;
	int size = cards.size();

	for(int i = 1; i <= size; ++i){
		card = cards.back(); 
		cards.pop_back();
		cards.insert(cards.begin() + randomInt(i), card);
	}
}

std::string Pack::toString(){
	std::string str = "";
	for(Item item : this->cards)
		str += (item + 'a' - 1);
	return str;
}



/////////////////////////////////////////////////////////////
//  metody tridy block:

//konstruktor

// random
Block::Block() : Block(randomShape(), randomInt(4)) {};
// nerandom
Block::Block(Shape shape, int rotation){
	this->item = NONE;
	this->shape = shape;
	this->orientation = 0;
	top = left = right = bottom = false;
	switch(shape){
		case Shape::I:
			top = bottom = true;
			break;
		case Shape::L:
			top = right = true;
			break;
		case Shape::T:
			left = right = bottom = true;
			break;
	}
	rotate(rotation);
}

void Block::rotate(int rotation){
	bool tmp;
	this->orientation += rotation;
	this->orientation %= 4;
	if(rotation > 0){ // to the left
		while(rotation--){
			tmp = left;
			left = top;
			top = right;
			right = bottom;
			bottom = tmp;
		}
	} else if(rotation < 0){ // to the right
		while(rotation++){
			tmp = right;
			right = top;
			top = left;
			left = bottom;
			bottom = tmp;
		}
	}
}


std::string Block::toString(){
	std::string str = "";
	str += "\n";
	str += "\t+---+";
	str += "\n";
	str += isTop() ? "\t|# #|" : "\t|###|" ;
	str += "\n";
	str += isLeft() ? "\t| " : "\t|#";
	str += item ? item + 'a' - 1 : ' ';
	str += isRight() ? " |" : "#|";
	str += "\n";
	str += isBottom() ? "\t|# #|" : "\t|###|";
	str += "\n";
	str += "\t+---+";
	str += "\n";
	
	return str;
}



////////////////////////////////////////////////////////////////////
// metody tridy Board

/**
 * konstruktor
 * @param size velikost bludiste
 */
Board::Board(int size = 7){
	this->size = size;
	spareBlock = new Block;
	board = new Block** [size];

	for(int i = 0; i < size; ++i){			
		board[i] = new Block* [size];
		for (int j = 0; j < size; ++j){
			// rohy
			if(i == 0 && j == 0){ // levy horni
				board[i][j] = new Block(Shape::L, RIGHT);
				continue;
			}
			if(i == 0 && j == size-1){ // pravy horni
				board[i][j] = new Block(Shape::L, 2*RIGHT);
				continue;
			}
			if(i == size-1 && j == 0){ //levy dolni
				board[i][j] = new Block(Shape::L, 0);
				continue;
			}
			if(i == size-1 && j == size-1){ // pravy dolni
				board[i][j] = new Block(Shape::L, LEFT);
				continue;
			}
			// strany
			if(i == 0 && j%2 == 0){ // horni sude
				board[i][j] = new Block(Shape::T, 0);
				continue;
			}
			if(i == size-1 && j%2 == 0){ //dolni sude
				board[i][j] = new Block(Shape::T, 2*LEFT);
				continue;
			}
			if(j == 0 && i%2 == 0){ //leve sude
				board[i][j] = new Block(Shape::T, LEFT);
				continue;
			}
			if(j == size-1 && i%2 == 0){ // prave sude
				board[i][j] = new Block(Shape::T, RIGHT);
				continue;
			}
			// ostatni fixni
			if(i%2 == 0 && j%2 == 0){
				board[i][j] = new Block(Shape::T, (i+j)/2);
				continue;
			}
			//random
			board[i][j] = new Block;

		}
	}

}

Board::Board(std::string format){

	int pos = 0;
	int size = (format[pos] - 'A' + 2) * 2 + 1;
	pos++;

	this->size = size;
	board = new Block** [size];
	Shape shape;

	for(int i = 0; i < size; ++i){			
		board[i] = new Block* [size];
		for (int j = 0; j < size; ++j){	
			switch(format[pos]){
				case 'I':
					shape = Shape::I;
					break;
				case 'L':
					shape = Shape::L;
					break;
				case 'T':
					shape = Shape::T;
					break;
				default:
					shape = Shape::O;
					break;
			}

			board[i][j] = new Block(shape, format[pos+1] - '0');
			board[i][j]->setItem(format[pos+2] - 'a');

			pos += 3;
		}
	}

	switch(format[pos]){
		case 'I':
			shape = Shape::I;
			break;
		case 'L':
			shape = Shape::L;
			break;
		case 'T':
			shape = Shape::T;
			break;
		default:
			shape = Shape::O;
			break;
	}
	spareBlock = new Block(shape, format[pos+1] - '0');
	spareBlock->setItem(format[pos+2] - 'a'); 
}

// destrucotr
Board::~Board(){
	for (int i = 0; i < size; ++i){
		for (int j = 0; j < size; ++j){
			delete board[i][j]; 
		}
		delete[] board[i];
	}
	delete[] board;

	delete spareBlock;
}

#define HASH "#"
#define FF(n) (figurestack.size() > n ? ((figurestack[n]->pos.x == i && figurestack[n]->pos.y == j) ? colortos(figurestack[n]->getColor()) : HASH ) : HASH) 

std::string Board::toString(){
	std::string str = "";
	// vrchni sipecky
	str += "   ";
	for (int n = 0; n < size/2; n++){
		str += "       |";
	}
	str += "\n";
	str += "   ";
	for (int n = 0; n < size/2; n++){
		str += "       |";
	}
	str += "\n";
	str += "   ";
	for (int n = 0; n < size/2; n++){
		str += "       V";
	}
	str += "\n\n";

	// konec vrchnich sipecek
	str += "    +";
	for (int j = 0; j < size; ++j){
		str += "---+";
	}
	str += "\n";
	for (int i = 0; i < size; ++i){
		str += "    |";
		for (int j = 0; j < size; ++j){
			str += board[i][j]->isTop() ? FF(0)+" "+FF(2)+"|" : FF(0)+"#"+FF(2)+"|";
		}
		str += "\n";

		if(i%2 == 1){ // sipecka nalevo 
			str += "--> |";
		}else{
			str += "    |";
		}

		for (int j = 0; j < size; ++j){
			str += board[i][j]->isLeft() ? " " : "#";
			str += board[i][j]->getItem()?(board[i][j]->getItem() + '`'):' ';
			str += board[i][j]->isRight() ? " |" : "#|";
		}
		if(i%2 == 1){ // sipecka napravo 
			str += " <--\n";
		}else{
			str += "\n";
		}
		
		str += "    |";
		for (int j = 0; j < size; ++j){
			str += board[i][j]->isBottom() ? FF(3)+" "+FF(1)+"|" : FF(3)+"#"+FF(1)+"|";
		}
		str += "\n    +";
		for (int j = 0; j < size; ++j){
			str += "---+";
		}
		str += "\n";
	}

	// sipecky dole
	str += "\n   ";
	for (int n = 0; n < size/2; n++){
		str += "       ^";
	}
	str += "\n";
	str += "   ";
	for (int n = 0; n < size/2; n++){
		str += "       |";
	}
	str += "\n";
	str += "   ";
	for (int n = 0; n < size/2; n++){
		str += "       |";
	}
	str += "\n";


	str += spareBlock->toString();
	
	return str;
}


bool Board::pickUpItem(Coords pos, Item card){
	if(board[pos.x][pos.y]->getItem() == card){
		board[pos.x][pos.y]->setItem(NONE);
		return true;
	} else {
		return false;
	}
}

bool Board::pickUpItem(Item card){
	for(int i = 0; i < size; ++i){			
		for (int j = 0; j < size; ++j){
			if(board[i][j]->getItem() == card){
				board[i][j]->setItem(NONE);
				return true;
			}	
		}
	}

	return false;
}


// rozmistit predmety po hracim poli
bool Board::placeItems(std::vector<Item> * items){
	Coords pos;
	// if ((size * size) < items->size())
	// 	return false;
	for(Item item : *items){
		do {
			pos.x = randomInt(size);
			pos.y = randomInt(size);
		} while(board[pos.x][pos.y]->getItem() != NONE);
		board[pos.x][pos.y]->setItem(item);
	}
	return true;
}


// umisti figurku na hraci pole
bool Board::placeFigure(Figure * figure){
	int len = figurestack.size();
	if(len >= 4){
		return false;
	}
	switch(len){
		case 0:
			figure->pos.x = 0;
			figure->pos.y = 0;
			break;
		case 1:
			figure->pos.x = size-1;
			figure->pos.y = size-1;
			break;
		case 2:
			figure->pos.x = 0;
			figure->pos.y = size-1;
			break;
		case 3:
			figure->pos.x = size-1;
			figure->pos.y = 0;
			break;
	}
	figurestack.push_back(figure);
	return true;

};

bool Board::shift(std::string data){
	if(data.size() != 2){
		return false;
	}

	int n = (data[1] - 'A')*2 + 1;
	Direction dir;
	switch(data[0]){
		case '0':
			dir = Direction::DOWN;
			break;
		case '1':
			dir = Direction::LEFT;
			break;
		case '2':
			dir = Direction::UP;
			break;
		case '3':
			dir = Direction::RIGHT;
			break;
	}
	return shift(dir, n);
}

bool Board::shift(Direction to, unsigned i){
	Block * poppedBlock;
	if(i >= size || i%2 == 0){ // nejde shiftovat
		return false;
	}
	if(to == Direction::RIGHT){
		poppedBlock = board[i][size-1];
		for (int j = size-1; j > 0 ; --j){
			board[i][j] = board[i][j-1];
		}
		board[i][0] = spareBlock;
	}
	if(to == Direction::LEFT){
		poppedBlock = board[i][0];
		for (int j = 0; j < size-1; ++j){
			board[i][j] = board[i][j+1];
		}
		board[i][size-1] = spareBlock;
	}
	if(to == Direction::DOWN){
		poppedBlock = board[size-1][i];
		for (int j = size-1; j > 0 ; --j){
			board[j][i] = board[j-1][i];
		}
		board[0][i] = spareBlock;
	}
	if(to == Direction::UP){
		poppedBlock = board[0][i];
		for (int j = 0; j < size-1; ++j){
			board[j][i] = board[j+1][i];
		}
		board[size-1][i] = spareBlock;
	}

	for(auto fig : figurestack){
		if(fig->pos.y == i && to == Direction::DOWN ){
			fig->pos.x++;
			fig->pos.x %= size;
		}
		if(fig->pos.y == i && to == Direction::UP ){
			fig->pos.x--;
			fig->pos.x += size;
			fig->pos.x %= size;
		}
		if(fig->pos.x == i && to == Direction::RIGHT ){
			fig->pos.y++;
			fig->pos.y %= size;
		}
		if(fig->pos.x == i && to == Direction::LEFT ){
			fig->pos.y--;
			fig->pos.y += size;
			fig->pos.y %= size;
		}
	}

	spareBlock = poppedBlock;
	return true;
}

bool Board::canPass(Coords startpos, Coords endpos){
	if(endpos.x >= size || endpos.y >= size || startpos.x >= size || startpos.y >= size)
		return false;

	Block* startBlock = board[startpos.x][startpos.y];
	Block* endBlock = board[endpos.x][endpos.y];

	// end je nad start blokem
	if(startpos.y == endpos.y && startpos.x - endpos.x == 1){
		return (startBlock->isTop() && endBlock->isBottom());
	}
	// end je pod start blokem
	if(startpos.y == endpos.y && endpos.x - startpos.x == 1){
		return (startBlock->isBottom() && endBlock->isTop());
	}
	// end je napravo od start bloku
	if(startpos.x == endpos.x && endpos.y - startpos.y == 1){
		return (startBlock->isRight() && endBlock->isLeft());
	}
	// end je nalevo od start bloku
	if(startpos.x == endpos.x && startpos.y - endpos.y == 1){
		return (startBlock->isLeft() && endBlock->isRight());
	}

	return false;
}

bool isInQueue(Coords coordinates, std::deque<Coords>* queue){
	for(auto pos : *queue){
		if(pos == coordinates){
			return true;
		}
	}

	return false;
}

bool Board::isConnected(Coords startPos, Coords endPos){
	std::deque<Coords> open;
	std::deque<Coords> closed;

	if(canPass(startPos, endPos))
		return true;

	open.push_back(startPos);

	Coords actPos;
	Coords topPos;
	Coords rightPos;
	Coords leftPos;
	Coords bottomPos;

	while(!open.empty()){
		actPos = open.front();
		open.pop_front();
		
		// kontrola vrchniho bloku
		topPos.x = actPos.x - 1;
		topPos.y = actPos.y; 
		
		if(canPass(topPos, actPos) && !isInQueue(topPos, &closed) && !isInQueue(topPos, &open)){
			if(topPos == endPos){
				return true;
			}
			//std::cout << "Push TOP" << std::endl;
			open.push_back(topPos);
		}

		// kontrola praveho bloku
		rightPos.x = actPos.x;
		rightPos.y = actPos.y + 1;

		if(canPass(rightPos, actPos) && !isInQueue(rightPos, &closed) && !isInQueue(rightPos, &open)){
			if(rightPos == endPos){
				return true;
			}
			//std::cout << "Push RIGHT" << std::endl;
			open.push_back(rightPos);
		}

		// kontrola leveho bloku
		leftPos.x = actPos.x;
		leftPos.y = actPos.y - 1;

		if(canPass(leftPos, actPos) && !isInQueue(leftPos, &closed) && !isInQueue(leftPos, &open)){
			if(leftPos == endPos){
				return true;
			}
			//std::cout << "Push LEFT" << std::endl;
			open.push_back(leftPos);
		}

		// kontrola spodniho bloku
		bottomPos.x = actPos.x + 1;
		bottomPos.y = actPos.y;

		if(canPass(bottomPos, actPos) && !isInQueue(bottomPos, &closed) && !isInQueue(bottomPos, &open)){
			if(bottomPos == endPos){
				return true;
			}
			//std::cout << "Push BOTTOM" << std::endl;
			open.push_back(bottomPos);
		}


		// presun do closed pole po rozvinutí uzlu
		closed.push_back(actPos);
	}

	return false;
}


std::string Board::toFormat(){
	std::string str = "";
	str += (((size-1)/2-2) + 'A');
	for(int i = 0; i < size; ++i){			
		for (int j = 0; j < size; ++j){
			str += shapetoc(board[i][j]->getShape());
			str += ('0' + board[i][j]->getRotation());
			str += ('a' + board[i][j]->getItem());
		}
	}
	str += shapetoc(spareBlock->getShape());
	str += ('0' + spareBlock->getRotation());
	str += ('a' + spareBlock->getItem());

	return str;
}

bool Board::canDoAnyMove(Coords actPos, Coords forbiddenPos){
	Coords dest;
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			dest = {i,j};
			if(dest != actPos && dest != forbiddenPos && isConnected(actPos, dest))
				return true;
		}
	}
	return false;
}