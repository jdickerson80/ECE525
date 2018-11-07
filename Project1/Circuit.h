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

	typedef std::list< unsigned int > IntList;

	struct Node
	{
		NodeTypes::Enum type;
		int fanoutNumber;
		int faninNumber;
		int output;
		IntList fanIn;
		IntList fanOut;

		Node();
	};

	struct Cell
	{
		int block;
		IntList nets;

		Cell();
	};

	struct Net
	{
		IntList cells;
		int nA;
		int nB;

		Net();
	};

	typedef Node Graph[ MAX_NODES ];
	typedef Cell CellVector[ MAX_NODES ];
	typedef std::vector< Net >  NetVector;

public:

	Circuit( FILE* inputFile );

	void populateCellandNetVectors();

	void printCellArray() const;

	void printGraph() const;

	void printNetArray() const;

	size_t maxNodeID() const;

	const Graph& graph() const;

	const CellVector& cellVector() const;

	const NetVector& netVector() const;
private:

	size_t _numberOfCells;
	CellVector _cellVector;
	IntList _inputVector;
	Graph _graph;
	size_t _maxNodeID;
	NetVector _netVector;
	IntList _outputVector;

	int readCircuit( FILE* inputFile );
};


#endif // CIRCUIT_H
