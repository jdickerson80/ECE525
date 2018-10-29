#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdio.h>
#include <list>
#include <vector>

#define MAX_NODES ( 25000 )
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

	typedef std::list< int > IntList;

	struct Node
	{
		NodeTypes::Enum type;
		int fanoutNumber;
		int faninNumber;
		int output;
		IntList fanIn;
		IntList fanOut;

		Node();

//		Node( NodeTypes::Enum type );
	};

	struct Cell
	{
		IntList nets;
		int block;

		Cell();
	};

	struct Net
	{
		IntList cells;
		int nA;
		int nB;

		Net();
	};

	Circuit( FILE* inputFile );


	typedef Node Graph[ MAX_NODES ];
	typedef std::vector< Cell > CellVector;
	typedef std::vector< Net >  NetVector;

	void printGraph() const;
private:
	CellVector _cellVector;
	IntList _inputVector;
	Graph _graph;
	int _maxNodeID;
	NetVector _netVector;
	IntList _outputVector;

	int readCircuit( FILE* inputFile );

};


#endif // CIRCUIT_H
