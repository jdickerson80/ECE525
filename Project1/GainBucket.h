#ifndef GAINBUCKET_H
#define GAINBUCKET_H

#include <vector>

#include "FMAlgorithm.h"

class GainBucket
{
public:

	typedef std::list< FMAlgorithm::Cell* > CellList;
	typedef std::vector< CellList > Bucket;

private:

	Bucket _bucket;
	int _maximumGain;
	int _minimumGain;

	static void printCellList( const CellList& list );
public:

	GainBucket();

	void addCell( FMAlgorithm::Cell* cell );

	FMAlgorithm::Cell* maxGainCell() const;

	int minimumGain() const;

	FMAlgorithm::Cell* removeHighestCellGain();

	void print() const;

	bool isEmpty() const;

	void emptyBucket();
};

#endif // GAINBUCKET_H
