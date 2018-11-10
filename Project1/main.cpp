#include <stdio.h>

#include "AdjacencyList.h"
#include "FMAlgorithm.h"
#include "CommandLineArgumentParser.h"

int main( int argc, char* argv[] )
{
	if ( argc < 2 )
	{
		CommandLineArgumentParser::printUsage();
		exit( EXIT_FAILURE );
	}

	FILE* inputFile	 = NULL;

	if ( CommandLineArgumentParser::parseCommandLineArgument( argc, argv, &inputFile ) )
	{
		CommandLineArgumentParser::printUsage();
		exit( EXIT_FAILURE );
	}

	AdjacencyList adjacencyList( inputFile );
	fclose( inputFile );

	FMAlgorithm fMAlgorithm( &adjacencyList, true );

	exit( EXIT_SUCCESS );
}

