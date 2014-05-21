#include "ConfigurationElement.h"
#include <iostream>

using namespace std;

	ufo::ConfigurationElement::ConfigurationElement( type t, bool fromTemplate)
: elementType(t), fromTemplate(fromTemplate), name(), parameters(), children()
{
}

ufo::ConfigurationElement::~ConfigurationElement()
{
	for ( std::vector <ConfigurationElement*>::const_iterator it= children.begin() ;
			it != children.end(); ++it )
	{
		delete (*it);
	}
}

void ufo::ConfigurationElement::print() const
{
	cout << ( fromTemplate?"fromTemplate ":"immediate " );
	switch (elementType)
	{
		case FLOCK:
			cout << "FLOCK " ;
			break;
		case PILOT:
			cout << "PILOT " ;
			break;
		case BEHAVIOUR:
			cout << "BEHAVIOUR " ;
			break;
		case STEERABLE:
			cout << "STEERABLE " ;
			break;
	}
	cout << name << " {" << endl;
	//print parameters:
	cout << "parameters {" <<endl;
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it= parameters.begin() ;
			it != parameters.end(); ++it )
	{
		cout << (*it).first << " = \"" << (*it).second << "\"" << endl;
	}
	cout << "}" << endl;
	//print children
	cout << "children {" <<endl;
	for ( std::vector <ConfigurationElement*>::const_iterator it= children.begin() ;
			it != children.end(); ++it )
	{
		(*it)->print();
	}
	cout << "}" << endl;
	cout << "} /* " << name << " */" << endl;	
}
