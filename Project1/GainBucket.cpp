#include "GainBucket.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MinGain ( -12 )
#define MaxGain ( 12 )
#define Offset ( MaxGain )

void GainBucket::printCellList( const GainBucket::CellList &list)
{
	GainBucket::CellList::const_iterator it = list.begin();

	while ( it != list.end() )
	{
		printf( "%lu ", (*it)->cellNumber );
		++it;
	}
}

GainBucket::GainBucket()
	: _maximumGain( 0 )
	, _minimumGain( 0 )
{
	size_t size = ( MaxGain - MinGain ) + 1;

	for ( size_t i = 0; i < size; ++i )
	{
		_bucket.push_back( CellList() );
	}
}

void GainBucket::addCell( FMAlgorithm::Cell* cell )
{
	int gain = cell->gain;

	if ( gain > _maximumGain )
	{
		_maximumGain = gain;
	}
	else if ( gain < _minimumGain )
	{
		_minimumGain = gain;
	}

	_bucket[ Offset - gain ].push_back( cell );
}

FMAlgorithm::Cell* GainBucket::maxGainCell() const
{
	return _bucket[ Offset - _maximumGain ].front();
}

int GainBucket::minimumGain() const
{
	return _minimumGain;
}

FMAlgorithm::Cell* GainBucket::removeHighestCellGain()
{
	if ( _maximumGain < _minimumGain )
	{
		return NULL;
	}

	CellList::iterator nodeIterator;
	CellList::const_iterator endIterator;

	for ( int i = _maximumGain; i >= MinGain; --i )
	{
		std::size_t index = static_cast< std::size_t >( Offset - i );

		if ( _bucket[ index ].empty() )
		{
			continue;
		}
		nodeIterator = _bucket[ index ].begin();
		endIterator = _bucket[ index ].end();

		while ( nodeIterator != endIterator )
		{
			if ( !(*nodeIterator)->lock )
			{
				_bucket[ index ].erase( nodeIterator );
				_maximumGain = i;
				return (*nodeIterator);
			}
			++nodeIterator;
		}
	}

	return NULL;
}

void GainBucket::print() const
{
	for ( std::size_t i = 0; i < _bucket.size(); ++i )
	{
		if ( _bucket[ i ].empty() )
		{
			continue;
		}

		printf("%i\t", Offset - (int)i );
		printCellList( _bucket[ i ] );
		printf("\n");
	}
}

bool GainBucket::isEmpty() const
{
	return _maximumGain < _minimumGain;
}

void GainBucket::emptyBucket()
{
	for ( std::size_t i = 0; i < _bucket.size(); ++i )
	{
		_bucket[ i ].clear();
	}
}
