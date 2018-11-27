#include "IntegerList.h"

#include <stdio.h>

void printList( const IntList& list )
{
	IntList::const_iterator it = list.begin();

	while ( it != list.end() )
	{
		printf( "%lu ", (*it) );
		++it;
	}
}
