#ifndef FMALGORITHM_H
#define FMALGORITHM_H

#include <vector>

#include "IntegerList.h"

#define MAX_NODES ( 25000 )

class AdjacencyList;

/**
 * @brief	The FMAlgorithm class is based on C.M. Fiduccia and R.M. Mattheyses's paper
 *			"A Linear-Time Heuristic for Improving Network Partions". It follow the paper
 *			very closely, but does differ a little.
 */
class FMAlgorithm
{
public:

	/**
	 * @brief	The Cell struct contains all of the data to describe a cell a.k.a.
	 *			a logic gate.
	 */
	struct Cell
	{
		int block;
		int gain;
		int lock;
		IntList nets;
		std::size_t cellNumber;

		Cell();
	};

	/**
	 * @brief	The Net struct contains all of the data to describe the nets of a design.
	 *			The list contains all of the cells on the net. nA and nB is the number
	 *			of nets in block a and b.
	 */
	struct Net
	{
		IntList cells;
		int nA;
		int nB;

		Net();
	};

	/**
	 * Typedefs for a more concise form of the types
	 */
	typedef Cell CellVector[ MAX_NODES ];
	typedef std::vector< Net > NetVector;
	typedef std::vector< Cell* > Partition;

public:

	/**
	 * @brief FMAlgorithm constructs an instance of the FM Algorithm
	 * @param adjacencyList pointer to the adjacency list to use
	 * @param	shouldExecute if true, the algorithm executes immediately,
	 *			otherwise it will wait for execute to be called
	 */
	FMAlgorithm( const AdjacencyList* const adjacencyList, bool shouldExecute = false );

	/**
	 * @brief execute executes the whole algorithm
	 */
	void execute();

	/**
	 * @brief printCellArray prints the cell array
	 */
	void printCellArray() const;

	/**
	 * @brief printNetArray prints the net array
	 */
	void printNetArray() const;

	/**
	 * @brief printPartitions prints the partitions
	 */
	void printPartitions() const;

	/**
	 * @brief cost getter
	 * @return cost
	 */
	int cost() const;

	/**
	 * @brief maxNodeID getter
	 * @return max node ID
	 */
	std::size_t maxNodeID() const;

	/**
	 * @brief cellVector getter
	 * @return reference to the cell vector
	 */
	const CellVector& cellVector() const;

	/**
	 * @brief netVector getter
	 * @return reference to the net vector
	 */
	const NetVector& netVector() const;

private:

	const AdjacencyList* const _adjacencyList;
	CellVector _cellVector;
	int _cost;
	NetVector _netVector;
	Partition _partitionA;
	Partition _partitionB;

private:

	/**
	 * @brief calculateCost calculates the cost
	 */
	inline void calculateCost();

	/**
	 * @brief calculateInitialGains calculates the initial gains
	 */
	inline void calculateInitialGains();

	/**
	 * @brief sortParitions sorts the partitions in descending order
	 */
	inline void sortParitions();

	/**
	 * @brief populateCellandNetVectors fills the net and cell vectors
	 * @todo break into smaller methods
	 */
	inline void populateCellandNetVectors();

	/**
	 * @brief calculateNetPartition calculates the number of nets in partition a and b for a given net
	 * @param net to check
	 */
	inline void calculateNetPartition( Net* net );
};

#endif // FMALGORITHM_H
