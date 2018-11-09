#ifndef FMALGORITHM_H
#define FMALGORITHM_H

#include <stdio.h>
#include <vector>

#include <AdjacencyList.h>

class FMAlgorithm
{
public:

	struct Cell
	{
		int block;
		int gain;
		int lock;
		int cellNumber;
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

	typedef Cell CellVector[ MAX_NODES ];
	typedef std::vector< Net > NetVector;
	typedef std::vector< Cell* > Partition;

	FMAlgorithm( FILE* inputFile );

	void calculateGains();

	void populateCellandNetVectors();

	void printCellArray() const;

	void printGraph() const;

	void printNetArray() const;

	void printPartitions() const;

	void sortParitions();

	void calculateCost();

	int cost() const;

	size_t maxNodeID() const;

	const CellVector& cellVector() const;

	const NetVector& netVector() const;
private:

	AdjacencyList _adjacencyList;
	CellVector _cellVector;
	int _cost;
	NetVector _netVector;
	Partition _partitionA;
	Partition _partitionB;
};

#endif // FMALGORITHM_H
