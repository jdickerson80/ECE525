#include <stdio.h>

#include "Circuit.h"
#include "CommandLineArgumentParser.h"

int main( int argc, char* argv[] )
{
	if ( argc < 2 )
	{
		CommandLineArgumentParser::printUsage();
		exit( EXIT_FAILURE );
	}

	FILE* inputFile	 = NULL;
	FILE* outputFile = NULL;

	if ( CommandLineArgumentParser::parseCommandLineArgument( argc, argv, &inputFile, &outputFile ) )
	{
		CommandLineArgumentParser::printUsage();
		exit( EXIT_FAILURE );
	}

	Circuit circuit( inputFile );

	circuit.populateCellandNetVectors();

	circuit.printGraph();

	printf( "------------------------\n" );

	circuit.printCellArray();

	printf( "------------------------\n" );

	circuit.printNetArray();


//	printf("%p\n", inputFile );

	exit( EXIT_SUCCESS );
}

