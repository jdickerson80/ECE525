#ifndef COMMANDLINEARGUMENTPARSER_H
#define COMMANDLINEARGUMENTPARSER_H

#include <string>
#include <vector>

class CommandLineArgumentParser
{
public:

	struct UsageArguments
	{
		enum
		{
			Help		= 'h',
			InputFile	= 'i',
			OutputFile	= 'o'
		};
	};

	static int parseCommandLineArgument( int argc, char*const* argv, FILE** inputFile, FILE** outputFile );

	static void printUsage();

	~CommandLineArgumentParser();
private:
	CommandLineArgumentParser();
};

#endif // COMMANDLINEARGUMENTPARSER_H
