#ifndef ADJACENCYLIST_H
#define ADJACENCYLIST_H

#include <list>
#include <stdio.h>

#define MAX_NODES ( 25000 )

typedef std::list< unsigned int > IntList;

/**
 * @brief The AdjacencyList class represents the circuit
 */
class AdjacencyList
{
public:

	/**
	 * @brief The NodeTypes struct holds all of the known nodes
	 */
	struct NodeTypes
	{
		enum Enum
		{
			Input = 1, And, Nand, Or, Nor,
			Not, Output, Buffer, EMPTY
		};
	};

	/**
	 * @brief The Node struct contains all of the data to describe a node
	 */
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

	/**
	 * @brief AdjacencyList constructs an adjacency list
	 * @param inputFile
	 */
	AdjacencyList( FILE* inputFile );

	/**
	 * @brief graph getter
	 * @return reference to the graph
	 */
	const Graph& graph() const;

	void printGraph() const;

	/**
	 * @brief inputVector getter
	 * @return reference to the input vector
	 */
	const IntList& inputVector() const;

	/**
	 * @brief outputVector
	 * @return reference to the output vector
	 */
	const IntList& outputVector() const;

	/**
	 * @brief numberOfCells in the graph
	 * @return number of cells in the graph
	 */
	size_t numberOfCells() const;

	/**
	 * @brief maxNodeID getter
	 * @return the last node's id
	 */
	size_t maxNodeID() const;

private:

	Graph _graph;
	IntList _inputVector;
	size_t _numberOfCells;
	size_t _maxNodeID;
	IntList _outputVector;
};

#endif // ADJACENCYLIST_H
