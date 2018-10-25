#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <list>

class Circuit
{
public:

	struct NodeTypes
	{
		enum Enum
		{
			Input	= 1,
			And,
			Nand,
			Or,
			Nor,
			Not,
			Output,
			Buffer,
			EMPTY
		};
	};

	typedef std::list< int > LIST;

	struct Node
	{
		NodeTypes::Enum type;
		int fanoutNumber;
		int faninNuber;
		int output;
		LIST fanIn;
		LIST fanOut;

		Node();
	};

	struct Cell
	{
		LIST* nets;
		int BLK;

		Cell();
	};

	struct Net
	{
		LIST* cells;
		int Nb,Na;

		Net();
	};

	Circuit();
};

#endif // CIRCUIT_H
