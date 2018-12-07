#include "FMAlgorithm.h"

#include "AdjacencyList.h"
#include "GainBucket.h"
#include <string.h>

#define EmptyBlock ( 10 )
#define PrintSeparator ( { printf( "------------------------\n" ); })

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
	calculateInitialGains();

	calculatePartitions();

	printCellArray();
	PrintSeparator;
	printNetArray();
	printf("Cost = %i\n", cost() );
}

void FMAlgorithm::calculateInitialGains()
{
	size_t maxNodeID = _adjacencyList->maxNodeID();

	// loop through the cell vector
	for ( size_t i = 0; i <= maxNodeID; ++i )
	{
		// if the cell block is empty or locked
		if ( _cellVector[ i ].block == EmptyBlock )
		{
			// next pass
			continue;
		}

		calculateCellGain( &_cellVector[ i ] );

		if ( _cellVector[ i ].block == 0 )
		{
			_aBucket->addCell( &_cellVector[ i ] );
		}
		else
		{
			_bBucket->addCell( &_cellVector[ i ] );
		}
	}

	calculateCost();
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
			++_partitionBSize;
		}
		else // partitionCounter <= a's threshold
		{
			// put it on block "a"
			_cellVector[ i ].block = 0;

			// put it in the a partition
			++_partitionASize;
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
	// if nA and nB is empty
	if ( net->nA == 0 && net->nB == 0 )
	{
		IntList::const_iterator nodeIterator = net->cells.begin();
		IntList::const_iterator nodeEnd = net->cells.end();
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

void FMAlgorithm::calculateCellGain( FMAlgorithm::Cell* cell )
{
	int from;
	int to;
	int fn;
	int tn;
	Net* tempNet;
	// get the start and end of the nets
	IntList::const_iterator nodeIterator = cell->nets.begin();
	IntList::const_iterator nodeEnd = cell->nets.end();

	// clear the gain
	cell->gain = 0;

	// get the from and to
	from = cell->block;
	to = !cell->block;

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
			++cell->gain;
		}

		// if tn equals 0
		if ( tn == 0 )
		{
			// decrement the gain
			--cell->gain;
		}

		// move to the next node
		++nodeIterator;
	}
}

void FMAlgorithm::calculatePartitions()
{
	Cell* swapCell = NULL;
	GainBucket* swapBucket = NULL;
	IntList::const_iterator nodeIterator;
	IntList::const_iterator nodeEnd;
	int iterations = 0;
	bool isABucketActive;
	int lowestCost = _cost;
	int bestCost = _cost;
	Cell* bestCellArray = new Cell[ MAX_NODES ];
	NetVector bestNetVector;// = new NetVector[ _netVector.size() ];

	memcpy( bestCellArray, &_cellVector, sizeof( _cellVector ) );

	while ( true )
	{
		++iterations;
		if ( _partitionASize > _partitionBSize )
		{
			swapBucket	= _aBucket;
			swapCell	= swapBucket->removeHighestCellGain();
			isABucketActive = true;
		}
		else
		{
			swapBucket	= _bBucket;
			swapCell	= swapBucket->removeHighestCellGain();
			isABucketActive = false;
		}

		if ( swapCell == NULL )
		{
			if ( bestCost <= lowestCost )
			{
				memcpy( &_cellVector, bestCellArray, sizeof( _cellVector ) );
				_netVector = bestNetVector;
				calculateCost();
				break;
			}
			bestCost = lowestCost;
			memcpy( &_cellVector, bestCellArray, sizeof( _cellVector ) );
			initializeGainBuckets();
			continue;
		}

		swapAndLockCellPartitions( swapCell );

		nodeIterator = swapCell->nets.begin();
		nodeEnd = swapCell->nets.end();

		while ( nodeIterator != nodeEnd )
		{
			if ( swapCell->block == 0 )
			{
				--_netVector[ (*nodeIterator) - 1 ].nB;
				++_netVector[ (*nodeIterator) - 1 ].nA;
			}
			else
			{
				--_netVector[ (*nodeIterator) - 1 ].nA;
				++_netVector[ (*nodeIterator) - 1 ].nB;
			}

			++nodeIterator;
		}

		calculateCellGain( swapCell );
		calculateCost();
		if ( isABucketActive )
		{
			_bBucket->addCell( swapCell );
		}
		else
		{
			_aBucket->addCell( swapCell );
		}

		if ( _cost < lowestCost )
		{
			lowestCost = _cost;
			memcpy( bestCellArray, &_cellVector, sizeof( _cellVector ) );
			bestNetVector = _netVector;
		}
	}
}

void FMAlgorithm::swapAndLockCellPartitions( FMAlgorithm::Cell* cell )
{
	if ( cell->block == 0 )
	{
		--_partitionASize;
		++_partitionBSize;
	}
	else
	{
		++_partitionASize;
		--_partitionBSize;
	}

	cell->block = !cell->block;
	cell->lock = true;
}

void FMAlgorithm::initializeGainBuckets()
{
	_aBucket->emptyBucket();
	_bBucket->emptyBucket();

	size_t maxNodeID = _adjacencyList->maxNodeID();
	_partitionASize = 0;
	_partitionBSize = 0;

	// loop through the cell vector
	for ( size_t i = 0; i <= maxNodeID; ++i )
	{
		// if the cell block is empty or locked
		if ( _cellVector[ i ].block == EmptyBlock )
		{
			// next pass
			continue;
		}

		if ( _cellVector[ i ].block == 0 )
		{
			_aBucket->addCell( &_cellVector[ i ] );
			++_partitionASize;
		}
		else
		{
			_bBucket->addCell( &_cellVector[ i ] );
			++_partitionBSize;
		}
	}
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
