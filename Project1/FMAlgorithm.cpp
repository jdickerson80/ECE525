#include "FMAlgorithm.h"

#define EmptyBlock ( 10 )

namespace {
void bubbleSort( FMAlgorithm::Partition& partition )
{
	FMAlgorithm::Cell* temp;
	size_t size = partition.size();

	// loop through the whole array
	for ( size_t x = 0; x < size; x++ )
	{
		// loop through array size - 1
		for ( size_t y = 0; y < size - 1; y++ )
		{
			// if the current element is less than the next element
			if ( partition[ y ]->gain < partition[ y + 1 ]->gain )
			{
				// store the next element
				temp = partition[ y + 1 ];

				// the next element equals the current element
				partition[ y + 1 ] = partition[ y ];

				// the current element equals the old next element
				partition[ y ] = temp;
			}
		}
	}
}

void printList( const IntList& list )
{
	IntList::const_iterator it = list.begin();

	while ( it != list.end() )
	{
		printf( "%i ", (*it) );
		++it;
	}
}
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


FMAlgorithm::FMAlgorithm( FILE* inputFile )
	: _adjacencyList( inputFile )
	, _cost( 0 )
{

}

void FMAlgorithm::calculateGains()
{
	IntList::const_iterator nodeIterator;
	IntList::const_iterator nodeEnd;
	Net* tempNet;
	size_t maxNodeID = _adjacencyList.maxNodeID();
	int from;
	int to;
	int fn;
	int tn;

	for ( size_t i = 0; i <= maxNodeID; ++i )
	{
		if ( _cellVector[ i ].block == EmptyBlock || _cellVector[ i ].lock )
		{
			continue;
		}
		nodeIterator = _cellVector[ i ].nets.begin();
		nodeEnd = _cellVector[ i ].nets.end();
		_cellVector[ i ].gain = 0;
		from = _cellVector[ i ].block;
		to = ~_cellVector[ i ].block & 0x1;

//		printf("blk %i f %i, t %i\n", _cellVector[ i ].block, from, to );

		while ( nodeIterator != nodeEnd )
		{
			tempNet = &_netVector[ (*nodeIterator) - 1 ];
			fn = from == 0 ? tempNet->nA : tempNet->nB;
			tn = to   == 0 ? tempNet->nA : tempNet->nB;
//			printf("\tnet %i, a %i, b %i, fn %i, tn %i\n", (*nodeIterator), tempNet->nA, tempNet->nB, fn, tn );
			if ( fn == 1 )
			{
				++_cellVector[ i ].gain;
			}
			if ( tn == 0 )
			{
				--_cellVector[ i ].gain;
			}

			++nodeIterator;
		}
	}
}

void FMAlgorithm::populateCellandNetVectors()
{
	size_t netVectorNumber;
	Net tempNet;
	const AdjacencyList::Node* tempNode;
	IntList::const_iterator nodeIterator;
	IntList::const_iterator nodeEnd;
	size_t maxNodeID = _adjacencyList.maxNodeID();
	size_t aBucketCellThreshold =  ( maxNodeID / 2 ) - 1;
	const AdjacencyList::Graph& graph = _adjacencyList.graph();

	for ( size_t i = 0; i <= maxNodeID; ++i )
	{
		tempNode = &graph[ i ];

		if ( tempNode->type == AdjacencyList::NodeTypes::EMPTY  )
		{
			continue;
		}

		if ( tempNode->fanOut.empty() )
		{
			_cellVector[ i ].cellNumber = i;

			if ( i > aBucketCellThreshold )
			{
				_partitionB.push_back( &_cellVector[ i ] );
			}
			else
			{
				_partitionA.push_back( &_cellVector[ i ] );
			}
			continue;
		}

		tempNet.nA = 0;
		tempNet.nB = 0;
		tempNet.cells.clear();
		netVectorNumber = _netVector.size() + 1;
		tempNet.cells.push_back( i );

		if ( i > aBucketCellThreshold )
		{
			++tempNet.nB;
			_cellVector[ i ].block = 1;
			_partitionB.push_back( &_cellVector[ i ] );
		}
		else
		{
			++tempNet.nA;
			_cellVector[ i ].block = 0;
			_partitionA.push_back( &_cellVector[ i ] );
		}

		_cellVector[ i ].cellNumber = i;
		_cellVector[ i ].nets.push_back( netVectorNumber );

		nodeIterator = tempNode->fanOut.begin();
		nodeEnd = tempNode->fanOut.end();

		while ( nodeIterator != nodeEnd )
		{
			tempNet.cells.push_back( (*nodeIterator) );
			_cellVector[ (*nodeIterator) ].nets.push_back( netVectorNumber );
			if ( (*nodeIterator) > aBucketCellThreshold )
			{
				++tempNet.nB;
				if ( _cellVector[ (*nodeIterator) ].block == EmptyBlock )
				{
					_cellVector[ (*nodeIterator)].block = 1;
				}
			}
			else
			{
				++tempNet.nA;
				if ( _cellVector[ (*nodeIterator) ].block == EmptyBlock )
				{
					_cellVector[ (*nodeIterator) ].block = 0;
				}
			}
			++nodeIterator;
		}

		_netVector.push_back( tempNet );
	}
}

void FMAlgorithm::printCellArray() const
{
	printf("Gain\tBlock\tCell\tLock\tNets\n");
	size_t maxNodeID = _adjacencyList.maxNodeID();
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

void FMAlgorithm::printGraph() const
{
	printf("NODE\tGATE\tIn #\tOut #\tFanin\tFanout\n");
	size_t maxNodeID = _adjacencyList.maxNodeID();
	const AdjacencyList::Graph& graph = _adjacencyList.graph();

	for ( unsigned int i = 0; i <= maxNodeID; ++i )
	{
		if ( graph[ i ].type != AdjacencyList::NodeTypes::EMPTY )
		{
			const AdjacencyList::Node& node = graph[ i ];
			printf("%d\t%d\t%d\t%d\t", i, node.type, node.faninNumber, graph[ i ].fanoutNumber);
			printList( node.fanIn );
			printf("\t");
			printList( node.fanOut );
			printf("\n");
		}
	}
	printf("MaxNodeID=%lu\nTOT_CELLS=%lu\n", maxNodeID, _adjacencyList.numberOfCells() );
}

void FMAlgorithm::printNetArray() const
{
	printf("Na\tNb\tCell\tNet\n");
	for ( size_t i = 0; i < _netVector.size(); ++i )
	{
		printf("%i\t%i\t%lu\t", _netVector[ i ].nA, _netVector[ i ].nB, i + 1 );
		printList( _netVector[ i ].cells );
		printf("\n");
	}
}

void FMAlgorithm::printPartitions() const
{
	printf("PA\tGain\t\n");
	for ( size_t i = 0; i < _partitionA.size(); ++i )
	{
		printf("%i\t%i\t\n", _partitionA[ i ]->cellNumber, _partitionA[ i ]->gain );
	}

	printf( "------------------------\n" );

	printf("PB\tGain\t\n");
	for ( size_t i = 0; i < _partitionB.size(); ++i )
	{
		printf("%i\t%i\t\n", _partitionB[ i ]->cellNumber, _partitionB[ i ]->gain );
	}
}

void FMAlgorithm::sortParitions()
{
	bubbleSort( _partitionA );
	bubbleSort( _partitionB );
}

void FMAlgorithm::calculateCost()
{
	_cost = 0;
	size_t size = _netVector.size();

	for ( size_t i = 0; i < size; ++i )
	{
		if ( _netVector[ i ].nA == 0 || _netVector[ i ].nB == 0 )
		{
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
