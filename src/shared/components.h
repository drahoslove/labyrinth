#pragma once

#include <string>
#include <deque>
#include <vector>
#include "debug.cpp"


int randomInt(int max);

typedef char Item;

const Item NONE = 0;
const Item XX = 4;

const int LEFT = +1;
const int RIGHT = -1;

enum class Color {
	INVISIBLE,
	
	RED,
	GREEN,
	BLUE,
	YELLOW,

	ORANGE,
	BLACK,
	WHITE,
	GREY,
	PURPLE,
};


inline Color operator++( Color& x ) { return x = (Color)(((int)(x) + 1)); }

enum class Shape {
	I,
	L,
	T,
	O, // uzavreny block, nepouziva se
};

// pro shiftovani
enum class Direction {
	LEFT,
	RIGHT,
	UP,
	DOWN,
};


//souradnice policka bludiste
class Coords{
public:
	Coords(){
		this->x = 0;
		this->y = 0;
	};

	Coords(int x, int y){
		this->x = x;
		this->y = y;
	};
	unsigned int x;
	unsigned int y;

	bool operator==(Coords &pos2){
		return (this->x == pos2.x && this->y == pos2.y);
	} 
};

class Pack {
	std::vector<Item> cards;

public:
	// vychozi
	Pack() : Pack(12) {};

	// custom
	Pack(unsigned int count){
		for (Item i = 1; i <= count; ++i)
			cards.push_back(i);
	}

	std::vector<Item> * get(){
		return &cards;
	}

	void shuffle();

	std::string toString();

};


// figurka
class Figure {

	Color color;
public:
	Figure () {
		this->color = Color::INVISIBLE;
	}
	Figure (Color color){
		this->color = color;
	}
	Color getColor(){
		return color;
	}

	Coords pos;
};

// policko bludiste
class Block {
private:
	bool top;
	bool right;
	bool left;
	bool bottom;

	// pro ucely vykreslovani:
	Shape shape;
	unsigned int orientation;

	
public:
	Item item;
	// constructory
	Block();
	Block(Shape, int = 0);

	// dalsi metody
	void rotate(int);

	bool isTop(){
		return top;
	}
	bool isBottom(){
		return bottom;
	}
	bool isLeft(){
		return left;
	}
	bool isRight(){
		return right;
	}
	Shape getShape(){
		return shape;
	}
	int getRotation(){
		return orientation;
	}
	Item getItem(){
		return item;
	}

	std::string toString();

};

// hraci deska
class Board {

	int size;
	Block*** board;
	Block* spareBlock;

	std::vector<Figure *> figurestack;

	bool canPass(Coords, Coords);

public:
	Board(int);
	Board(std::string);
	~Board();

	std::string toString();

	std::string toFormat();


	bool rotate(int n = 1){
		spareBlock->rotate(n*LEFT);
	}
	
	bool shift(Direction, unsigned);
	bool shift(std::string);

	bool isConnected(Coords, Coords); 

	// umistit itemy
	bool placeItems(std::vector<Item> *);
	// umisti figurku na hraci pole
	bool placeFigure(Figure *);

};



