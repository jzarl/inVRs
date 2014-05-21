#undef DEBUG
#include "Configurator.h"

#include <iostream>

#include "ConfigurationElement.h"
#include "Debug.h"
#include "UfoDB.h"

using namespace std;

using namespace ufo;

	ufo::Configurator::Configurator ()
: plugindirs(), elements(), templates(), flocks(), pilots()
{
}

ufo::Configurator::~Configurator ()
{
	// the ConfigurationElements are owned by the Configurator, so we have to delete them:
	// (object pointers inside STL containers are not deleted)

	//elements:
	for ( std::vector <ConfigurationElement*>::const_iterator it = elements.begin(); it != elements.end(); ++it )
	{
		delete (*it);
	}
	//templates:
	for ( std::map <std::string,ConfigurationElement *>::const_iterator it = templates.begin(); it != templates.end(); ++it )
	{
		delete (*it).second;
	}
}
const bool ufo::Configurator::bind ()
{
	PRINT("DEBUG: ufo::Configurator::bind()\n");
	// first, add the plugin directories:
	UfoDB::the()->swapPluginDirectories(plugindirs);
	// now get to work:
	for ( std::vector <ConfigurationElement*>::iterator it = elements.begin(); it != elements.end(); ++it )
	{ //for each element:
		ConfigurationElement *elem= (*it);
		switch (elem->elementType)
		{
			Flock *f;
			Pilot *p;
			case ConfigurationElement::FLOCK:
				f = bindFlock(elem);
				if ( ! f )
				{
					internalCleanup();
					return false;
				}
				flocks.push_back(f);
				break;
			case ConfigurationElement::PILOT:
				p = bindPilot(elem);
				if ( ! p )
				{
					internalCleanup();
					return false;
				}
				pilots.push_back(p);
				break;
			default:
				cout << "bind(): Warning: Illegal type at top-level! Ignoring..." <<endl;
				break;
		}
	}
	return true;
}

#if 0
const vector <Flock *> ufo::Configurator::getFlocks() const
{
	return flocks;
}

const vector <Pilot *> ufo::Configurator::getPilots() const
{
	return pilots;
}
#endif

void ufo::Configurator::print() const
{
	cout << "==== BEGIN configuration:" << endl;
	cout << "--- Elements:" <<endl;
	for ( std::vector <ConfigurationElement*>::const_iterator it = elements.begin(); it != elements.end(); ++it )
	{
		(*it)->print();
	}
	cout << "--- Templates:" << endl;
	for ( std::map <std::string,ConfigurationElement *>::const_iterator it = templates.begin(); it != templates.end(); ++it )
	{
		cout << "template " << (*it).first << " " ;
		(*it).second->print();
	}
	cout << "==== END configuration." << endl;
}

string ufo::Configurator::lookupTemplateName (ConfigurationElement *elem) const
{
	PRINT("DEBUG: lookupTemplateName(): element name is %s\n",elem->name.c_str());
	//check if a template lookup is neccessary:
	if ( elem->fromTemplate )
	{
		string origname = elem->name;
		// N.B.: if a template is used, the name is the template name!
		// lookup template:
		elem = templates.find(elem->name)->second;
		if ( elem == 0 )
		{
			//template not found:
			cout << "lookupTemplateName(): missing template for element " 
				<< origname <<"!" << endl;
			return "";
		}
		if ( elem->fromTemplate )
		{
			PRINT("DEBUG: lookupTemplateName(): Recursive template lookup...\n");
			return lookupTemplateName(elem);
		}
	}
	return elem->name;
}

std::vector<std::pair<std::string,std::string> > * ufo::Configurator::lookupTemplateParameters (ConfigurationElement *elem) const
{
	std::vector<std::pair<std::string,std::string> > * ret = new std::vector<std::pair<std::string,std::string> >(elem->parameters);
	//check if a template lookup is neccessary:
	if ( elem->fromTemplate )
	{
		string origname = elem->name;
		// N.B.: if a template is used, the name is the template name!
		// lookup template:
		elem = templates.find(elem->name)->second;
		if ( elem == 0 )
		{
			//template not found:
			cout << "lookupTemplateParameters(): missing template for element " 
				<< origname <<"!" << endl;
			delete ret;
			return 0;
		}
		// template was found -> add its parameters:
		ret->insert(ret->end(), elem->parameters.begin(), elem->parameters.end());
		// further indirection?
		if ( elem->fromTemplate )
		{
			std::vector<std::pair<std::string,std::string> > *tpl = lookupTemplateParameters(elem);
			ret->insert(ret->end(), tpl->begin(), tpl->end());
			delete tpl;
		}
	}
	return ret;
}

std::vector <ConfigurationElement*>* ufo::Configurator::lookupTemplateChildren (ConfigurationElement *elem) const
{
	std::vector <ConfigurationElement*> * ret = new std::vector <ConfigurationElement*>(elem->children);
	//check if a template lookup is neccessary:
	if ( elem->fromTemplate )
	{
		string origname = elem->name;
		// N.B.: if a template is used, the name is the template name!
		// lookup template:
		elem = templates.find(elem->name)->second;
		if ( elem == 0 )
		{
			//template not found:
			cout << "lookupTemplateChildren(): missing template for element " 
				<< origname <<"!" << endl;
			delete ret;
			return 0;
		}
		// template was found -> add its children:
		ret->insert(ret->end(), elem->children.begin(), elem->children.end());
		// further indirection?
		if ( elem->fromTemplate )
		{
			std::vector <ConfigurationElement*> *tpl = lookupTemplateChildren(elem);
			ret->insert(ret->end(), tpl->begin(), tpl->end());
			delete tpl;
		}
	}
	return ret;
}


Flock * ufo::Configurator::bindFlock (ConfigurationElement *elem)
{
	Flock *flock;
	Flock::factoryType factory;

	// get factory:
	string name = lookupTemplateName(elem);
	if ( name == "" )
	{
		cout << "bindFlock(): Template lookup failed!" <<endl;
		return 0;
	}
	factory = UfoDB::the()->getFlockFactory(name);
	if ( factory == 0)
	{
		cout << "bindFlock(): No factory for type " << name << "!" << endl;
		return 0;
	}

	//create Flock:
	// if this lookup fails, we have a serius problem(because the previous name lookup went fine):
	std::vector<std::pair<std::string,std::string> > *tmp = lookupTemplateParameters(elem);
	flock = factory(tmp);
	delete tmp;
	if ( ! flock )
	{
		cout << "bindFlock(): Creation of flock failed!" <<endl;
		return 0;
	}
	//from here on, always remember to delete the flock on failure:

	//a flock element can have Pilot children:
	std::vector <ConfigurationElement*> *children = lookupTemplateChildren(elem);
	for ( std::vector <ConfigurationElement*>::iterator it = children->begin(); 
			it != children->end(); ++it )
	{
		ConfigurationElement *elem= (*it);
		switch (elem->elementType)
		{
			Pilot *p;
			case ConfigurationElement::PILOT:
				p = bindPilot(elem,flock);
				if ( ! p )
				{
					flock->destroy();
					return 0;
				}
				flock->push_back(p);
				break;
			default:
				cout << "bindFlock(): Warning: Illegal child type for type Flock! Ignoring..." << endl;
		}
	}
	delete children;
	// everything went fine:
	return flock;
}

Pilot * ufo::Configurator::bindPilot (ConfigurationElement *elem, Flock *flock )
{
	Pilot *pilot;
	Pilot::factoryType factory;

	// get factory:
	string name = lookupTemplateName(elem);
	if ( name == "" )
	{
		cout << "bindPilot(): Template lookup failed!" <<endl;
		return 0;
	}
	factory = UfoDB::the()->getPilotFactory(name);
	if ( factory == 0 )
	{
		cout << "bindPilot(): No factory for type " << name << "!" << endl;
		return 0;
	}

	//a pilot element must have behaviour and steerable children:
	Behaviour *b = 0;
	Steerable *st = 0;

	std::vector <ConfigurationElement*> *children = lookupTemplateChildren(elem);
	for ( std::vector <ConfigurationElement*>::iterator it = children->begin(); 
			it != children->end(); ++it )
	{
		ConfigurationElement *elem= (*it);
		switch (elem->elementType)
		{
			case ConfigurationElement::BEHAVIOUR:
				if ( ! b)
				{
					// be lazy on detecting an error here - we will catch it before Pilot creation...
					b = bindBehaviour(elem);
				} else {
					cout << "bindPilot(): Warning: superfluous Behaviour defined. Ignoring..." <<endl;
				}
				break;
			case ConfigurationElement::STEERABLE:
				if ( ! st)
				{
					// be lazy on detecting an error here - we will catch it before Pilot creation...
					st = bindSteerable(elem);
				} else {
					cout << "bindPilot(): Warning: superfluous Steerable defined. Ignoring..." <<endl;
				}
				break;
			default:
				cout << "bindPilot(): Warning: Illegal child type for type Pilot! Ignoring..." << endl;
		}
	}
	if ( b == 0 || st == 0 )
	{
		if ( b ) 
			b->destroy();
		else 
			cout << "bindPilot(): Pilot has no Behaviour!" <<endl;
		if ( st ) 
			st->destroy();
		else
			cout << "bindPilot(): Pilot has no Steerable!" <<endl;
		// give the user some context:
		cout << "bindPilot(): Corresponding ConfigurationElement of Pilot:" <<endl;
		elem->print();
		cout << "bindPilot(): Complete list of children (direct and from templates):" <<endl;
		for ( std::vector <ConfigurationElement*>::iterator it = children->begin(); 
				it != children->end(); ++it )
		{
			(*it)->print();
		}
		delete children;
		return 0;
	}
	delete children;
	//create pilot:
	//if we have been called by a flock, bindFlock() called us with a flock parameter != 0
	// if this lookup fails, we have a serius problem(because the previous name lookup went fine):
	std::vector<std::pair<std::string,std::string> > *tmp = lookupTemplateParameters(elem);
	pilot = factory(tmp,b,st,flock);
	delete tmp;
	if ( ! pilot )
	{
		cout << "bindPilot(): Creation of Pilot failed!" <<endl;
		return 0;
	}
	// everything went fine:
	return pilot;
}

Behaviour * ufo::Configurator::bindBehaviour (ConfigurationElement *elem)
{
	Behaviour *behaviour;
	Behaviour::factoryType factory;

	// get factory:
	string name = lookupTemplateName(elem);
	if ( name == "" )
	{
		cout << "bindBehaviour(): Template lookup failed!" <<endl;
		return 0;
	}
	factory = UfoDB::the()->getBehaviourFactory(name);
	if ( factory == 0 )
	{
		cout << "bindBehaviour(): No factory for type " << name << "!" << endl;
		return 0;
	}

	//a Behaviour element may have Behaviour children:
	vector<Behaviour*> bs;
	std::vector <ConfigurationElement*> *children = lookupTemplateChildren(elem);
	for ( std::vector <ConfigurationElement*>::iterator it = children->begin(); 
			it != children->end(); ++it )
	{
		ConfigurationElement *elem= (*it);
		Behaviour *b;
		switch (elem->elementType)
		{
			case ConfigurationElement::BEHAVIOUR :
				b = bindBehaviour(elem);
				if ( b )
				{
					bs.push_back(b);
				}
				break;
			default:
				cout << "bindBehaviour(): Warning: Illegal child type for type Behaviour! Ignoring..." << endl;
		}
	}
	delete children;
	PRINT("INFO: bindBehaviour(): Behaviour %s has %d children\n",elem->name.c_str(),bs.size());
	//create Behaviour:
	// if this lookup fails, we have a serius problem(because the previous name lookup went fine):
	std::vector<std::pair<std::string,std::string> > *tmp = lookupTemplateParameters(elem);
	behaviour = factory(bs,tmp);
	delete tmp;
	if ( ! behaviour )
	{
		cout << "bindBehaviour(): Creation of Behaviour failed!" <<endl;
		return 0;
	}
	// everything went fine:
	return behaviour;
}

Steerable * ufo::Configurator::bindSteerable (ConfigurationElement *elem)
{
	Steerable *steerable;
	Steerable::factoryType factory;

	// get factory:
	string name = lookupTemplateName(elem);
	if ( name == "" )
	{
		cout << "bindSteerable(): Template lookup failed!" <<endl;
		return 0;
	}
	factory = UfoDB::the()->getSteerableFactory(name);
	if ( factory == 0 )
	{
		cout << "bindSteerable(): No factory for type " << name << "!" << endl;
		return 0;
	}

	//create Steerable:
	// if this lookup fails, we have a serius problem(because the previous name lookup went fine):
	std::vector<std::pair<std::string,std::string> > *tmp = lookupTemplateParameters(elem);
	steerable = factory(tmp);
	delete tmp;
	if ( ! steerable )
	{
		cout << "bindSteerable(): Creation of Steerable failed!" << endl;
		return 0;
	}
	//from here on, always remember to delete the Steerable on failure:

	//a Steerable element can have no children, but let's not fail over this:
	// also, we don't bother about a template lookup in this case
	if (elem->children.size() != 0 )
	{
		cout << "bindSteerable(): Warning: Type Steerable may not have children! Ignoring..." << endl;
	}
	// everything went fine:
	return steerable;
}

void ufo::Configurator::internalCleanup()
{
	// delete flocks already created:
	for (vector <Flock *>::iterator it= flocks.begin();
			it != flocks.end(); ++it)
	{
		(*it)->destroy();
	}
	// delete pilots already created:
	for (vector <Pilot *>::iterator it= pilots.begin();
			it != pilots.end(); ++it)
	{
		(*it)->destroy();
	}
	// reset plugin directories in UfoDB:
	UfoDB::the()->swapPluginDirectories(plugindirs);
}
