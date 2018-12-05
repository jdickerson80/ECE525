#include "GainBucket.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MinGain ( -6 )
#define MaxGain ( 6 )
#define Offset ( MaxGain )

GainBucket::GainBucket()
{
	size_t size = ( MaxGain - MinGain ) + 1;

	for ( size_t i = 0; i < size; ++i )
	{
		_bucket.push_back( IntList() );
	}

//	printf("size = %lu\n", _bucket.size() );
}

//GainBucket::GainBucket( const FMAlgorithm::Partition& sortedPartition )
//{
//	int maxGain = sortedPartition[ 0 ]->gain;

//	_minimumGain = static_cast< std::size_t >( sortedPartition[ sortedPartition.size() - 1 ]->gain * -1 );

//	std::size_t delta = static_cast< std::size_t >( maxGain - sortedPartition[ sortedPartition.size() - 1 ]->gain ) + 1;
//	_highestGain = delta - 1;

//	for ( std::size_t i = 0; i < delta; ++i )
//	{
//		_bucket.push_back( IntList() );
//	}

//	printf("max %i min %d delta %i size %lu\n", maxGain, _minimumGain, Offset, _bucket.size() );
//	for ( std::size_t i = 0; i < sortedPartition.size(); ++i )
//	{
////		printf("%i, ", sortedPartition[ i ]->gain + minGain );
//		_bucket[ static_cast< std::size_t >( sortedPartition[ i ]->gain + (int)_minimumGain ) ].push_back( sortedPartition[ i ]->cellNumber );
//	}

////	for ( std::size_t i = 0; i < delta; ++i )
////	{
////		printList( _bucket[ i ] );
////		printf("\n");
//	//	}
//}

void GainBucket::addCell( const FMAlgorithm::Cell* const cell )
{
	int gain = cell->gain;

	if ( gain > _highestGain )
	{
		_highestGain = gain;
	}
	else if ( gain < _minimumGain )
	{
		_minimumGain = gain;
	}

	_bucket[ Offset - gain ].push_back( cell->cellNumber );
}

std::size_t GainBucket::maxGainCell() const
{
	return _bucket[ Offset - _highestGain ].front();
}

int GainBucket::minimumGain() const
{
	return _minimumGain;
}

bool GainBucket::removeHighestCellGain()
{
	printf("high %i low %i\n", Offset - _highestGain, Offset - _minimumGain );
	if ( _highestGain < _minimumGain )
	{
		return false;
	}

	_bucket[ Offset - _highestGain ].pop_front();

	if ( _bucket[ Offset - _highestGain ].empty() )
	{
		--_highestGain;
	}

	return true;
}

void GainBucket::print() const
{
	for ( std::size_t i = 0; i < _bucket.size(); ++i )
	{
		printf("%i\t", Offset - i );
		printList( _bucket[ i ] );
		printf("\n");
	}
}
