#include "Circuit.h"
#include <stdlib.h>

Circuit::Circuit()
{

}

Circuit::Node::Node()
	: type( NodeTypes::EMPTY )
	, fanoutNumber( 0 )
	, faninNuber( 0 )
	, output( 0 )
	, fanIn()
	, fanOut()
{

}

Circuit::Cell::Cell()
{

}

Circuit::Net::Net()
{

}
