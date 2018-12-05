#ifndef GAINBUCKET_H
#define GAINBUCKET_H

#include <vector>

#include "FMAlgorithm.h"

class GainBucket
{
public:

	typedef std::vector< IntList > Bucket;

private:

	Bucket _bucket;
	int _minimumGain;
	int _highestGain;

public:

	/**
	 * @brief GainBucket
	 * @param sortedPartition
	 * @todo this cannot work like this to maintain O(N) complexity. This adds another loop through the partition, so
	 *		it needs to be done WHILE creating the partitions. Just done like this because of the time crunch
	 */
//	GainBucket( const FMAlgorithm::Partition& sortedPartition );

	GainBucket();

	void addCell( const FMAlgorithm::Cell* const cell );

	std::size_t maxGainCell() const;

	int minimumGain() const;

	bool removeHighestCellGain();

	void print() const;
};

#endif // GAINBUCKET_H
