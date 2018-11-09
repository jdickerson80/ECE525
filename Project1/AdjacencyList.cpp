#include "AdjacencyList.h"

#include <string.h>
#include <stdlib.h>

AdjacencyList::NodeTypes::Enum assign_type(char *type)
{
	if(strcmp(type,"INPUT")==0)			return AdjacencyList::NodeTypes::Input;
	else if(strcmp(type,"AND")==0)		return AdjacencyList::NodeTypes::And;
	else if(strcmp(type,"NAND")==0)		return AdjacencyList::NodeTypes::Nand;
	else if(strcmp(type,"OR")==0)		return AdjacencyList::NodeTypes::Or;
	else if(strcmp(type,"NOR")==0)		return AdjacencyList::NodeTypes::Nor;
	else if(strcmp(type,"NOT")==0)		return AdjacencyList::NodeTypes::Not;
	else if(strcmp(type,"OUTPUT")==0)	return AdjacencyList::NodeTypes::Output;
	else if(strcmp(type,"BUFF")==0)		return AdjacencyList::NodeTypes::Buffer;
	else								return AdjacencyList::NodeTypes::EMPTY;
}


void charcat( char *s, char b )
{
	while(*s!='\0')
	{
		(*s++);
	}
	*s=b;
}

AdjacencyList::Node::Node()
	: type( NodeTypes::EMPTY )
	, fanoutNumber( 0 )
	, faninNumber( 0 )
	, output( 0 )
	, fanIn()
	, fanOut()
{

}

AdjacencyList::AdjacencyList( FILE* inputFile )
	: _maxNodeID( 0 )
{
#define NODE_BUFF_SIZE ( 10 )
#define MAX_LINE ( 100 )
	char  nodeTypeBuffer[NODE_BUFF_SIZE],nodeIDBuffer[NODE_BUFF_SIZE],linebuffer[MAX_LINE];
	int   i,node_no,node_count=0;
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
			_maxNodeID = nodeID;
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
}

const AdjacencyList::Graph &AdjacencyList::graph() const
{
	return _graph;
}

const IntList& AdjacencyList::inputVector() const
{
	return _inputVector;
}

const IntList& AdjacencyList::outputVector() const
{
	return _outputVector;
}

size_t AdjacencyList::numberOfCells() const
{
	return _numberOfCells;
}

size_t AdjacencyList::maxNodeID() const
{
	return _maxNodeID;
}
