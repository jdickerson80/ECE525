#ifndef ADJACENCYLIST_H
#define ADJACENCYLIST_H

#include <list>
#include <stdio.h>

#define MAX_NODES ( 25000 )

typedef std::list< unsigned int > IntList;

class AdjacencyList
{
public:

	struct NodeTypes
	{
		enum Enum
		{
			Input	= 1, And, Nand, Or,	Nor,
			Not, Output, Buffer, EMPTY
		};
	};

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

	typedef Node Graph[ MAX_NODES ];

public:

	AdjacencyList( FILE* inputFile );

	const Graph& graph() const;

	const IntList& inputVector() const;

	const IntList& outputVector() const;

	size_t numberOfCells() const;

	size_t maxNodeID() const;

private:

	Graph _graph;
	IntList _inputVector;
	size_t _numberOfCells;
	size_t _maxNodeID;
	IntList _outputVector;
};

#endif // ADJACENCYLIST_H
