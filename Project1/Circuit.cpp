#include "Circuit.h"
#include <stdlib.h>
#include <string.h>

void printList( const Circuit::IntList& list )
{
	Circuit::IntList::const_iterator it = list.begin();

	while ( it != list.end() )
	{
		printf( "%i ", (*it) );
		++it;
	}
}

Circuit::Node::Node()
	: type( NodeTypes::EMPTY )
	, fanoutNumber( 0 )
	, faninNumber( 0 )
	, output( 0 )
	, fanIn()
	, fanOut()
{

}

Circuit::Cell::Cell()
	: block( 0 )
{

}

Circuit::Net::Net()
	: nA( 0 )
	, nB( 0 )
{

}

Circuit::Circuit( FILE* inputFile )
	: _numberOfCells( 0 )
	, _maxNodeID( 0 )
{
	_maxNodeID = readCircuit( inputFile );
}

void Circuit::populateCellandNetVectors()
{
	size_t netVectorNumber;
	Net tempNet;
	Node* tempNode;
	Circuit::IntList::const_iterator nodeIterator;
	Circuit::IntList::const_iterator nodeEnd;
	size_t aBucketCellThreshold =  ( _maxNodeID / 2 ) - 1;
	printf("%lu\n", aBucketCellThreshold );

	for ( size_t i = 0; i <= _maxNodeID; ++i )
	{
		tempNode = &_graph[ i ];

		if ( tempNode->type == NodeTypes::EMPTY || tempNode->fanOut.empty() )
		{
			continue;
		}

		tempNet.nA = 0;
		tempNet.nB = 0;
		tempNet.cells.clear();
		netVectorNumber = _netVector.size() + 1;
		tempNet.cells.push_back( i );

		i > aBucketCellThreshold ? ++tempNet.nB : ++tempNet.nA;

		_cellVector[ i ].nets.push_back( netVectorNumber );

		nodeIterator = tempNode->fanOut.begin();
		nodeEnd = tempNode->fanOut.end();

		while ( nodeIterator != nodeEnd )
		{
			tempNet.cells.push_back( (*nodeIterator) );
			(*nodeIterator) > aBucketCellThreshold ? ++tempNet.nB : ++tempNet.nA;
			_cellVector[ (*nodeIterator) ].nets.push_back( netVectorNumber );
			++nodeIterator;
		}

		_netVector.push_back( tempNet );
	}
}

void Circuit::printCellArray() const
{
	printf("Cell\tBlock\tNets\n");
	for ( size_t i = 0; i <= _maxNodeID; ++i )
	{
		if ( _cellVector[ i ].nets.empty() )
		{
			continue;
		}

		printf("%lu\t%i\t", i, _cellVector[ i ].block );
		printList( _cellVector[ i ].nets );
		printf("\n");
	}
}


void Circuit::printGraph() const
{
	printf("NODE\tGATE\tIn #\tOut #\tFanin\tFanout\n");

	for ( unsigned int i = 0; i < MAX_NODES; ++i )
	{
		if ( _graph[ i ].type != NodeTypes::EMPTY )
		{
			const Node& node = _graph[ i ];
			printf("%d\t%d\t%d\t%d\t",i,node.type,node.faninNumber,_graph[ i ].fanoutNumber);
			printList( node.fanIn );
			printf("\t");
			printList( node.fanOut );
			printf("\n");
		}
	}
}

void Circuit::printNetArray() const
{
	printf("Net\tNa\tNb\tCells\n");
	for ( size_t i = 0; i < _netVector.size(); ++i )
	{
		printf("%lu\t%i\t%i\t", i + 1, _netVector[ i ].nA, _netVector[ i ].nB );
		printList( _netVector[ i ].cells );
		printf("\n");
	}
}

size_t Circuit::maxNodeID() const
{
	return _maxNodeID;
}

const Circuit::Graph& Circuit::graph() const
{
	return _graph;
}

const Circuit::CellVector& Circuit::cellVector() const
{
	return _cellVector;
}

const Circuit::NetVector& Circuit::netVector() const
{
	return _netVector;
}

void charcat( char *s, char b )
{
	while(*s!='\0')
	{
		*s++;
	}
	*s=b;
}

Circuit::NodeTypes::Enum assign_type(char *type)
{
	if(strcmp(type,"INPUT")==0)			return Circuit::NodeTypes::Input;
	else if(strcmp(type,"AND")==0)		return Circuit::NodeTypes::And;
	else if(strcmp(type,"NAND")==0)		return Circuit::NodeTypes::Nand;
	else if(strcmp(type,"OR")==0)		return Circuit::NodeTypes::Or;
	else if(strcmp(type,"NOR")==0)		return Circuit::NodeTypes::Nor;
	else if(strcmp(type,"NOT")==0)		return Circuit::NodeTypes::Not;
	else if(strcmp(type,"OUTPUT")==0)	return Circuit::NodeTypes::Output;
	else if(strcmp(type,"BUFF")==0)		return Circuit::NodeTypes::Buffer;
	else								return Circuit::NodeTypes::EMPTY;
}

int Circuit::readCircuit( FILE* inputFile )
{
#define NODE_BUFF_SIZE ( 10 )
#define MAX_LINE ( 100 )
	char  nodeTypeBuffer[NODE_BUFF_SIZE],nodeIDBuffer[NODE_BUFF_SIZE],linebuffer[MAX_LINE];
	int   i,node_no,node_count=0,max_node_id=0;
	int nodeID;

	do
		fgets(linebuffer,MAX_LINE,inputFile);
	while(linebuffer[0]=='#');

	while(!feof(inputFile))
	{
		bzero(nodeIDBuffer, NODE_BUFF_SIZE);
		bzero(nodeTypeBuffer, NODE_BUFF_SIZE);


		if(linebuffer[0]=='#' || linebuffer[0]=='\n')
			fgets(linebuffer,MAX_LINE,inputFile);

		/*------------------------Feeding the Inputs------------------------------*/

		if(linebuffer[0]=='I')
		{

			strcpy(nodeTypeBuffer,"INPUT");
			i=0;
			while(linebuffer[i]!='(')
				i++;

			while(linebuffer[i]!=')')
			{
				i++;
				charcat(nodeIDBuffer,linebuffer[i]);
			}

			nodeID = atoi( nodeIDBuffer );
			_graph[ nodeID ].type=assign_type( nodeTypeBuffer );
			_inputVector.push_back( nodeID );
			++_numberOfCells;
		}

		/*-------------------------Feeding the Outputs------------------------------*/

		else if(linebuffer[0]=='O')
		{
			strcpy(nodeTypeBuffer,"OUTPUT");
			i=0;
			while(linebuffer[i]!='(')
				i++;

			while(linebuffer[i]!=')')
			{
				i++;
				charcat(nodeIDBuffer,linebuffer[i]);
			}

			nodeID = atoi( nodeIDBuffer );
			Node& node = _graph[nodeID];
			node.type=assign_type(nodeTypeBuffer);
			node.fanoutNumber=0;
			node.output=1;
			_outputVector.push_back( nodeID );
			max_node_id = nodeID;
		}

		/*-----------------------Feeding the Internal Gates------------------------------*/
		else
		{
			i=0;

			while(linebuffer[i]!= '=')
			{
				charcat(nodeIDBuffer,linebuffer[i]);
				i++;
			}
			i++;

			while(linebuffer[i]!='(')
			{
				i++;
				if(linebuffer[i]!='(') charcat(nodeTypeBuffer,linebuffer[i]);
			}

			nodeID = atoi(nodeIDBuffer);
			_graph[ nodeID ].type=assign_type(nodeTypeBuffer);
			node_no = nodeID;
			bzero(nodeIDBuffer, NODE_BUFF_SIZE);
			bzero(nodeTypeBuffer, NODE_BUFF_SIZE);
			i++;

			while(linebuffer[i]!='\0')
			{
				if(linebuffer[i]!= ',')
					charcat(nodeIDBuffer,linebuffer[i]);


				if(linebuffer[i]== ',' || linebuffer[i]== ')')
				{
					nodeID = atoi(nodeIDBuffer);
					_graph[ node_no ].fanIn.push_back( nodeID );
					_graph[ nodeID ].fanOut.push_back( node_no);
					node_count = nodeID;
					bzero(nodeIDBuffer, NODE_BUFF_SIZE);
					i++;
				}
				i++;

				_graph[node_no].faninNumber = _graph[node_no].fanIn.size();
				_graph[node_count].fanoutNumber = _graph[node_count].fanOut.size();
			}

			++_numberOfCells;
		}/*------------------End of feeding gates----------------------------*/

		bzero(linebuffer, MAX_LINE);
		fgets(linebuffer,MAX_LINE,inputFile);

	}/*end while*/
	fclose(inputFile);
	return max_node_id;
}

void Circuit::handleNetAddition( int graphPosition )
{
	Net tempNet;
	tempNet.cells.push_back( graphPosition );

	Circuit::IntList::const_iterator it = _graph[ graphPosition ].fanOut.begin();

	while ( it != _graph[ graphPosition ].fanOut.end() )
	{
		tempNet.cells.push_back( (*it) );
		++it;
	}

	_netVector.push_back( tempNet );
}
