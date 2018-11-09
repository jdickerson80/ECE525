#include <stdio.h>

#include "FMAlgorithm.h"
#include "CommandLineArgumentParser.h"

#define PrintSeparator ( { printf( "------------------------\n" ); })
int main( int argc, char* argv[] )
{
	if ( argc < 2 )
	{
		CommandLineArgumentParser::printUsage();
		exit( EXIT_FAILURE );
	}

	FILE* inputFile	 = NULL;
	FILE* outputFile = stdout;

	if ( CommandLineArgumentParser::parseCommandLineArgument( argc, argv, &inputFile, &outputFile ) )
	{
		CommandLineArgumentParser::printUsage();
		exit( EXIT_FAILURE );
	}

	FMAlgorithm fMAlgorithm( inputFile );
	fclose( inputFile );

	fMAlgorithm.populateCellandNetVectors();
	fMAlgorithm.calculateGains();

	fMAlgorithm.printGraph();

	PrintSeparator;

	fMAlgorithm.printCellArray();

	PrintSeparator;

	fMAlgorithm.printNetArray();

	PrintSeparator;

	fMAlgorithm.printPartitions();
	fMAlgorithm.sortParitions();
	PrintSeparator;

	fMAlgorithm.printPartitions();

	PrintSeparator;

	fMAlgorithm.calculateCost();

	printf("Cost = %i\n", fMAlgorithm.cost() );

	exit( EXIT_SUCCESS );
}

