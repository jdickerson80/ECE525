#include "CommandLineArgumentParser.h"

#include <getopt.h>
#include <stdio.h>

using namespace std;

static char usage[] = \
	"Usage: FMAlgorithm [-h] [-i] [-o]\n" \
				"-h, --help\t\tprint this help message\n" \
				"-i, --InputFiles\tBenchmark file to read in\n";

// setup the long options
static struct option longOptions[] =
{
	{ "InputFiles",		required_argument,	0, CommandLineArgumentParser::UsageArguments::InputFile },
	{ "help",			no_argument,		0, CommandLineArgumentParser::UsageArguments::Help },
	{ 0,				0,					0, 0  }
};

int CommandLineArgumentParser::parseCommandLineArgument( int argc, char*const* argv, FILE** inputFile )
{
	int opt;

	// parse the user's arguments
	while ( ( opt = getopt_long( argc, argv, "hi:o:", longOptions, NULL ) ) != -1 )
	{
		switch ( opt )
		{
		case UsageArguments::InputFile:
			*inputFile = fopen( optarg, "r" );
			break;
		}
	}

	return *inputFile ? 0 : -1;
}

void CommandLineArgumentParser::printUsage()
{
	perror( usage );
}
