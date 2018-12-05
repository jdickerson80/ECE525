#include "FMAlgorithm.h"

#include "AdjacencyList.h"
#include "GainBucket.h"

#define EmptyBlock ( 10 )
#define PrintSeparator ( { printf( "------------------------\n" ); })

namespace {

/**
 * @brief bubbleSort
 * @param partition
 * @todo change sorting algorithm to one with less complexity
 */
//void bubbleSort( FMAlgorithm::Partition& partition )
//{
//	FMAlgorithm::Cell* temp;
//	size_t size = partition.size();

//	// loop through the whole array
//	for ( size_t x = 0; x < size; x++ )
//	{
//		// loop through array size - 1
//		for ( size_t y = 0; y < size - 1; y++ )
//		{
//			// if the current element is less than the next element
//			if ( partition[ y ]->gain < partition[ y + 1 ]->gain )
//			{
//				// store the next element
//				temp = partition[ y + 1 ];

//				// the next element equals the current element
//				partition[ y + 1 ] = partition[ y ];

//				// the current element equals the old next element
//				partition[ y ] = temp;
//			}
//		}
//	}
//}

}

FMAlgorithm::Cell::Cell()
	: block( EmptyBlock )
	, gain( 0 )
	, lock( 0 )
	, cellNumber( 0 )
{

}

FMAlgorithm::Net::Net()
	: nA( 0 )
	, nB( 0 )
{

}

FMAlgorithm::FMAlgorithm( const AdjacencyList* const adjacencyList, bool shouldExecute /* = false */ )
	: _aBucket( new GainBucket() )
	, _bBucket( new GainBucket() )
	, _adjacencyList( adjacencyList )
	, _cost( 0 )
	, _partitionASize( 0 )
	, _partitionBSize( 0 )
{
	if ( shouldExecute )
	{
		execute();
	}
}

FMAlgorithm::~FMAlgorithm()
{
	delete _aBucket;
	delete _bBucket;
}

void FMAlgorithm::execute()
{
	populateCellandNetVectors();

//	_adjacencyList->printGraph();
//	PrintSeparator;


	calculateInitialGains();
	printCellArray();
	PrintSeparator;

	printNetArray();
	PrintSeparator;

	printPartitions();
	PrintSeparator;

	sortParitions();
	PrintSeparator;

	printPartitions();
	PrintSeparator;

	populateGainBuckets();
	calculatePartitions();

	calculateCost();
	printf("Cost = %i\n", cost() );
}

void FMAlgorithm::calculateInitialGains()
{
	IntList::const_iterator nodeIterator;
	IntList::const_iterator nodeEnd;
	Net* tempNet;
	size_t maxNodeID = _adjacencyList->maxNodeID();
	int from;
	int to;
	int fn;
	int tn;

	// loop through the cell vector
	for ( size_t i = 0; i <= maxNodeID; ++i )
	{
		// if the cell block is empty or locked
		if ( _cellVector[ i ].block == EmptyBlock || _cellVector[ i ].lock )
		{
			// next pass
			continue;
		}

		// get the start and end of the nets
		nodeIterator = _cellVector[ i ].nets.begin();
		nodeEnd = _cellVector[ i ].nets.end();

		// clear the gain
		_cellVector[ i ].gain = 0;

		// get the from and to
		from = _cellVector[ i ].block;
		to = ~_cellVector[ i ].block & 0x1;

		// loop through the nets
		while ( nodeIterator != nodeEnd )
		{
			// get the corresponding net from the net list
			tempNet = &_netVector[ (*nodeIterator) - 1 ];

			// calculate the Na and Nb values
			calculateNetPartition( tempNet );

			// calculate f(n) and t(n)
			fn = from == 0 ? tempNet->nA : tempNet->nB;
			tn = to   == 0 ? tempNet->nA : tempNet->nB;

			// if fn equals 1
			if ( fn == 1 )
			{
				// increment the gain
				++_cellVector[ i ].gain;
			}

			// if tn equals 0
			if ( tn == 0 )
			{
				// decrement the gain
				--_cellVector[ i ].gain;
			}

			// move to the next node
			++nodeIterator;
		}

		if ( _cellVector[ i ].block == 0 )
		{
			_aBucket->addCell( &_cellVector[ i ] );
		}
		else
		{
			_bBucket->addCell( &_cellVector[ i ] );
		}
	}
}

void FMAlgorithm::populateCellandNetVectors()
{
	// init the necessary variables
	size_t netVectorNumber;
	Net tempNet;
	const AdjacencyList::Node* tempNode;
	IntList::const_iterator nodeIterator;
	IntList::const_iterator nodeEnd;
	size_t partitionCounter = 0;

	size_t maxNodeID = _adjacencyList->maxNodeID();
	size_t aBucketCellThreshold =  ( _adjacencyList->numberOfCells() / 2 ) % 2 == 0
			? ( _adjacencyList->numberOfCells() / 2 )
			: ( _adjacencyList->numberOfCells() / 2 ) + 1;

	const AdjacencyList::Graph& graph = _adjacencyList->graph();

	// loop through the adjacency list
	for ( size_t i = 0; i <= maxNodeID; ++i )
	{
		// get the node
		tempNode = &graph[ i ];

		// if the position is empty
		if ( tempNode->type == AdjacencyList::NodeTypes::EMPTY  )
		{
			// next
			continue;
		}

		// increment the counter
		++partitionCounter;

		// clear the temp net
		tempNet.nA = 0;
		tempNet.nB = 0;
		tempNet.cells.clear();

		// get the net vector number
		netVectorNumber = _netVector.size() + 1;
		tempNet.cells.push_back( i );

		// partitionCounter > a's threshold
		if ( partitionCounter > aBucketCellThreshold )
		{
			// put it on block "b"
			_cellVector[ i ].block = 1;

			// put it in the b partition
//			_partitionB.push_back( &_cellVector[ i ] );
			++_partitionBSize;
		}
		else // partitionCounter <= a's threshold
		{
			// put it on block "a"
			_cellVector[ i ].block = 0;

			// put it in the a partition
			++_partitionASize;
//			_partitionA.push_back( &_cellVector[ i ] );
		}

		// get the cell number
		_cellVector[ i ].cellNumber = i;

		// if this is an output, do not add it to the net vector
		if ( tempNode->fanOut.empty() )
		{
			continue;
		}

		// add it to the net
		_cellVector[ i ].nets.push_back( netVectorNumber );

		// get the beginning and end of the fan outs
		nodeIterator = tempNode->fanOut.begin();
		nodeEnd = tempNode->fanOut.end();

		// loop through them
		while ( nodeIterator != nodeEnd )
		{
			// add it to the cells
			tempNet.cells.push_back( (*nodeIterator) );

			// add it to the cell vector's net
			_cellVector[ (*nodeIterator) ].nets.push_back( netVectorNumber );

			// increment the iterator
			++nodeIterator;
		}

		// add the temp net to the net vector
		_netVector.push_back( tempNet );
	}
}

void FMAlgorithm::calculateNetPartition(Net *net )
{
	IntList::const_iterator nodeIterator = net->cells.begin();
	IntList::const_iterator nodeEnd = net->cells.end();

	// if nA and nB is empty
	if ( net->nA == 0 && net->nB == 0 )
	{
		// loop through the nets
		while ( nodeIterator != nodeEnd )
		{
			// if the cell is in block A
			if ( _cellVector[ (*nodeIterator) ].block == 0 )
			{
				// increment the nA
				++net->nA;
			}
			else // cell is in block B
			{
				// increment the nB
				++net->nB;
			}

			// increment the iterator
			++nodeIterator;
		}
	}
}

void FMAlgorithm::populateGainBuckets()
{
//	_aBucket = new GainBucket( _partitionA );
//	_bBucket = new GainBucket( _partitionB );
}

void FMAlgorithm::calculatePartitions()
{
	printf("A Highest = %lu\n", _aBucket->maxGainCell() );
	printf("B Highest = %lu\n", _bBucket->maxGainCell() );
	std::size_t swapCell = 0;
	GainBucket* swapBucket = nullptr;

	swapCell	= _partitionASize > _partitionBSize ? _bBucket->maxGainCell() : _aBucket->maxGainCell();
	swapBucket	= _partitionASize > _partitionBSize ? _bBucket : _aBucket;


	swapBucket->print();
	PrintSeparator;
	swapBucket->removeHighestCellGain();
	swapAndLockCellPartitions( &_cellVector[ swapCell ] );

	printCellArray();
	swapBucket->print();
	printf("A Highest = %lu\n", _aBucket->maxGainCell() );
	printf("B Highest = %lu\n", _bBucket->maxGainCell() );


//	printf("pre cell %i ", _cellVector[ swapCell ].block );


	//	printf("cell %lu block %i lock %i\n", swapCell, _cellVector[ swapCell ].block, _cellVector[ swapCell ].lock );
}

void FMAlgorithm::swapAndLockCellPartitions( FMAlgorithm::Cell* cell )
{
	if ( cell->block == 0 )
	{
		--_partitionASize;
	}
	else
	{
		--_partitionBSize;
	}

	cell->block = !cell->block;
	cell->lock = true;
}

void FMAlgorithm::printCellArray() const
{
	printf("Gain\tBlock\tCell\tLock\tNets\n");
	size_t maxNodeID = _adjacencyList->maxNodeID();
	for ( size_t i = 0; i <= maxNodeID; ++i )
	{
		if ( _cellVector[ i ].nets.empty() )
		{
			continue;
		}

		printf("%i\t%i\t%lu\t%i\t"
			   , _cellVector[ i ].gain
			   , _cellVector[ i ].block
			   , i
			   , _cellVector[ i ].lock );
		printList( _cellVector[ i ].nets );
		printf("\n");
	}
}

void FMAlgorithm::printNetArray() const
{
	printf("Na\tNb\tNet\tCell\n");
	for ( size_t i = 0; i < _netVector.size(); ++i )
	{
		printf("%i\t%i\t%lu\t", _netVector[ i ].nA, _netVector[ i ].nB, i + 1 );
		printList( _netVector[ i ].cells );
		printf("\n");
	}
}

void FMAlgorithm::printPartitions() const
{
//	printf("PA\tGain\t\n");
//	for ( size_t i = 0; i < _partitionA.size(); ++i )
//	{
//		printf("%lu\t%i\t\n", _partitionA[ i ]->cellNumber, _partitionA[ i ]->gain );
//	}

//	PrintSeparator;

//	printf("PB\tGain\t\n");
//	for ( size_t i = 0; i < _partitionB.size(); ++i )
//	{
//		printf("%lu\t%i\t\n", _partitionB[ i ]->cellNumber, _partitionB[ i ]->gain );
//	}
}

void FMAlgorithm::sortParitions()
{
//	bubbleSort( _partitionA );
//	bubbleSort( _partitionB );
}

void FMAlgorithm::calculateCost()
{
	// clear the cost
	_cost = 0;
	size_t size = _netVector.size();

	// loop through the net
	for ( size_t i = 0; i < size; ++i )
	{
		// if number of nets in a is not 0 and b is not zero
		if ( _netVector[ i ].nA != 0 && _netVector[ i ].nB != 0 )
		{
			// increment the cost
			++_cost;
		}
	}
}

int FMAlgorithm::cost() const
{
	return _cost;
}

const FMAlgorithm::CellVector& FMAlgorithm::cellVector() const
{
	return _cellVector;
}

const FMAlgorithm::NetVector& FMAlgorithm::netVector() const
{
	return _netVector;
}
