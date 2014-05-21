#include "UfoDB.h"

#include <iostream>
#include <unistd.h>

#include "Debug.h"
// for registering the default FlockFactory:
#include "Flock.h"
// for registering the default PilotFactory:
#include "Pilot.h"
#include "Plugin.h"

using namespace ufo;
using namespace std;

ufo::UfoDB *ufo::UfoDB::theSingleton = 0;

ufo::UfoDB *ufo::UfoDB::the()
{
	if ( theSingleton == 0 )
	{
		theSingleton = new UfoDB();
	}
	return theSingleton;
}

void ufo::UfoDB::reset()
{
	if ( theSingleton != 0 )
	{
		delete theSingleton;
		theSingleton = 0;
	}
}

ufo::UfoDB::UfoDB()
	: nextFlockSN(0), nextPilotSN(), pluginDirectories(), plugins(), flocks(), pilots()
{
	// register factories for builtin types:
	registerFlockType("Flock",&FlockFactory);
	registerPilotType("Pilot",&PilotFactory);
}

ufo::UfoDB::~UfoDB()
{
	// delete Flocks and Pilots:
	//PRINT("ufo::UfoDB::~UfoDB(): %d pilots.\n", pilots.size() );
	while ( ! pilots.empty() )
	{
		Pilot *p = pilots.back();
		pilots.pop_back();
		// delete Pilot without the Pilot deregistering from us!
		p->destroy(false);
	}
	while ( ! flocks.empty() )
	{
		Flock *f = flocks.back();
		flocks.pop_back();
		// delete Flock without the Flock deregistering from us!
		f->destroy(false);
	}

	//close plugins:
	for ( vector<Plugin*>::const_iterator it = plugins.begin(); it != plugins.end(); ++it )
	{
		delete (*it);
	}
	PRINT("ufo::UfoDB::~UfoDB(): UfoDB deleted.\n");
}

const unsigned int ufo::UfoDB::addFlock(Flock *f)
{
	flocks.push_back(f);
	return nextFlockSN++;
}
const unsigned int ufo::UfoDB::addPilot(Pilot *p, Flock* f)
{
	if ( f == 0 )
	{
		// if Pilot has no Flock, UfoDB is owner of it:
		pilots.push_back(p);
	}
	return nextPilotSN++;
}
void ufo::UfoDB::removeFlock(Flock *f)
{
	for ( vector<Flock *>::iterator it = flocks.begin(); it != flocks.end(); ++it)
	{
		if ( f == (*it) )
		{
			flocks.erase(it);
			return;
		}
	}
	cout << "ufo::UfoDB::removeFlock(): Flock not found!" <<endl;
}
void ufo::UfoDB::removePilot(Pilot *p)
{
	for ( vector<Pilot *>::iterator it = pilots.begin(); it != pilots.end(); ++it)
	{
		if ( p == (*it) )
		{
			pilots.erase(it);
			return;
		}
	}
	cout << "ufo::UfoDB::removePilot(): Pilot not found!" <<endl;
}


void ufo::UfoDB::addPluginDirectory( string dir )
{
	pluginDirectories.push_back(dir);
}

void ufo::UfoDB::swapPluginDirectories( vector<string> &dirs)
{
	pluginDirectories.swap(dirs);
}

void ufo::UfoDB::registerFlockType( string name, Flock::factoryType creator )
{
	PRINT("ufo::UfoDB::registerFlockType( %s )\n", name.c_str());
	flockFactories.insert( flockFMap::value_type(name,creator) );
}

void ufo::UfoDB::registerPilotType( string name, Pilot::factoryType creator )
{
	PRINT("ufo::UfoDB::registerPilotType( %s )\n", name.c_str());
	pilotFactories.insert( pilotFMap::value_type(name,creator));
}

void ufo::UfoDB::registerBehaviourType( string name, Behaviour::factoryType creator )
{
	PRINT("ufo::UfoDB::registerBehaviourType( %s )\n", name.c_str());
	behaviourFactories.insert( behaviourFMap::value_type(name,creator));
}

void ufo::UfoDB::registerSteerableType( string name, Steerable::factoryType creator )
{
	PRINT("ufo::UfoDB::registerSteerableType( %s )\n", name.c_str());
	steerableFactories.insert( steerableFMap::value_type(name,creator));
}

Flock::factoryType ufo::UfoDB::getFlockFactory( string name )
{
	if ( flockFactories.find(name) != flockFactories.end() )
	{
		return flockFactories[name];
	} else {
		// try to find a suitable plugin file:
		string filename = findPluginFile("libufoplugin_Flock_" + (name));
		if ( filename != "" )
		{
			Plugin *p;
			try 
			{
				// load plugin:
				p = new Plugin(filename,name,Plugin::FLOCK);
			} catch ( runtime_error &e ) {
				cout << "Plugin for type " << name << " found, but could not be loaded!" <<endl;
				cout << "Error was: \n" << e.what() <<endl;
				return 0;
			}
			if ( flockFactories.find(name) != flockFactories.end() )
			{
				// everything ok
				plugins.push_back(p);
				return flockFactories[name];
			} else {
				cout << "Plugin loaded, but still no factory for type " << name << "!" << endl;
				// delete unusable plugin:
				delete p;
				return 0;
			}
		} else {
			cout << "No suitable plugin file found for type " << name << "!" <<endl;
			return 0;
		}
	}
	// we shouldn't get here (all branches of the above 'if' return)
	assert(0);
}
Pilot::factoryType ufo::UfoDB::getPilotFactory( string name )
{
	if ( pilotFactories.find(name) != pilotFactories.end() )
	{
		return pilotFactories[name];
	} else {
		// try to find a suitable plugin file:
		string filename = findPluginFile("libufoplugin_Pilot_" + (name));
		if ( filename != "" )
		{
			Plugin *p;
			try 
			{
				// load plugin:
				p = new Plugin(filename,name,Plugin::PILOT);
			} catch ( runtime_error &e ) {
				cout << "Plugin for type " << name << " found, but could not be loaded!" <<endl;
				cout << "Error was: \n" << e.what() <<endl;
				return 0;
			}
			if ( pilotFactories.find(name) != pilotFactories.end() )
			{
				// everything ok
				plugins.push_back(p);
				return pilotFactories[name];
			} else {
				cout << "Plugin loaded, but still no factory for type " << name << "!" << endl;
				// delete unusable plugin:
				delete p;
				return 0;
			}
		} else {
			cout << "No suitable plugin file found for type " << name << "!" <<endl;
			return 0;
		}
	}
	// we shouldn't get here (all branches of the above 'if' return)
	assert(0);
}
Behaviour::factoryType ufo::UfoDB::getBehaviourFactory( string name )
{
	if ( behaviourFactories.find(name) != behaviourFactories.end() )
	{
		return behaviourFactories[name];
	} else {
		// try to find a suitable plugin file:
		string filename = findPluginFile("libufoplugin_Behaviour_" + (name));
		if ( filename != "" )
		{
			Plugin *p;
			try 
			{
				// load plugin:
				p = new Plugin(filename,name,Plugin::BEHAVIOUR);
			} catch ( runtime_error &e ) {
				cout << "Plugin for type " << name << " found, but could not be loaded!" <<endl;
				cout << "Error was: \n" << e.what() <<endl;
				return 0;
			}
			if ( behaviourFactories.find(name) != behaviourFactories.end() )
			{
				// everything ok
				plugins.push_back(p);
				return behaviourFactories[name];
			} else {
				cout << "Plugin loaded, but still no factory for type " << name << "!" << endl;
				// delete unusable plugin:
				delete p;
				return 0;
			}
		} else {
			cout << "No suitable plugin file found for type " << name << "!" <<endl;
			return 0;
		}
	}
	// we shouldn't get here (all branches of the above 'if' return)
	assert(0);
}
Steerable::factoryType ufo::UfoDB::getSteerableFactory( string name )
{
	if ( steerableFactories.find(name) != steerableFactories.end() )
	{
		return steerableFactories[name];
	} else {
		// try to find a suitable plugin file:
		string filename = findPluginFile("libufoplugin_Steerable_" + (name));
		if ( filename != "" )
		{
			Plugin *p;
			try 
			{
				// load plugin:
				p = new Plugin(filename,name,Plugin::STEERABLE);
			} catch ( runtime_error &e ) {
				cout << "Plugin for type " << name << " found, but could not be loaded!" <<endl;
				cout << "Error was: \n" << e.what() <<endl;
				return 0;
			}
			if ( steerableFactories.find(name) != steerableFactories.end() )
			{
				// everything ok
				plugins.push_back(p);
				return steerableFactories[name];
			} else {
				cout << "Plugin loaded, but still no factory for type " << name << "!" << endl;
				// delete unusable plugin:
				delete p;
				return 0;
			}
		} else {
			cout << "No suitable plugin file found for type " << name << "!" <<endl;
			return 0;
		}
	}
	// we shouldn't get here (all branches of the above 'if' return)
	assert(0);
}

void ufo::UfoDB::update(const float dt) const
{
	// update independent Pilots:
	for ( vector<Pilot *>::const_iterator it = pilots.begin(); it != pilots.end(); ++it)
	{
		(*it)->steer(dt);
	}
	// update Pilots in Flocks:
	for ( vector<Flock *>::const_iterator itf = flocks.begin(); itf != flocks.end(); ++itf)
	{
		(*itf)->update(dt);
	}
}

const vector <Flock *> &ufo::UfoDB::getFlocks() const
{
	return flocks;
}

Flock * ufo::UfoDB::getFlockBySN( const unsigned int sn) const
{
	// FIXME: use map
	for ( vector<Flock *>::const_iterator it = flocks.begin(); it != flocks.end(); ++it)
	{
		if ( (*it)->getSN() == sn )
		{
			return (*it);
		}
	}
	return 0;
}

const vector <Pilot *> &ufo::UfoDB::getPilots() const
{
	return pilots;
}

Pilot * ufo::UfoDB::getPilotBySN( const unsigned int sn) const
{
	// FIXME: use map
	// search independent Pilots:
	for ( vector<Pilot *>::const_iterator it = pilots.begin(); it != pilots.end(); ++it)
	{
		if ( (*it)->getSN() == sn )
		{
			return (*it);
		}
	}
	// search Pilots in Flocks:
	for ( vector<Flock *>::const_iterator itf = flocks.begin(); itf != flocks.end(); ++itf)
	{
		for ( Flock::const_iterator itp = (*itf)->begin(); itp != (*itf)->end(); ++itp)
		{
			if ( (*itp)->getSN() == sn )
			{
				return (*itp);
			}
		}
	}
	return 0;
}

vector <Pilot *> ufo::UfoDB::getPilotsByAttribute( const std::string &key, const std::string &value) const
{
	// FIXME:
	//  - return by value
	//  - inefficient search through all Flocks
	vector<Pilot *> ret;
	// search independent Pilots:
	for ( vector<Pilot *>::const_iterator it = pilots.begin(); it != pilots.end(); ++it)
	{
		if ( (*it)->getAttribute(key) == value )
		{
			ret.push_back( *it);
		}
	}
	// search Pilots in Flocks:
	for ( vector<Flock *>::const_iterator itf = flocks.begin(); itf != flocks.end(); ++itf)
	{
		for ( Flock::const_iterator itp = (*itf)->begin(); itp != (*itf)->end(); ++itp)
		{
			if ( (*itp)->getAttribute(key) == value )
			{
				ret.push_back( *itp);
			}
		}
	}
	return ret;
}

void ufo::UfoDB::print() const
{
	cout << "===BEGIN UfoDB:" <<endl;
	cout << "plugindirectories: " <<endl;
	for (vector<string>::const_iterator it = pluginDirectories.begin();
			it != pluginDirectories.end(); ++it)
	{
		cout << " -> " << (*it) << endl;
	}
	cout << "nextFlockSN: " << nextFlockSN <<endl;
	cout << "nextPilotSN: " << nextPilotSN <<endl;
	cout << "=== UfoDB Plugins:" <<endl;
	for (vector<Plugin*>::const_iterator it = plugins.begin();
			it != plugins.end(); ++it)
	{
		cout << " -> " << (*it)->pluginTypename() << "Version " 
			<< (*it)->pluginVersion()
			<< "-" << (*it)->pluginBranchname() 
			<< ": " << (*it)->pluginFilename() << endl;
	}
	cout << "=== UfoDB Flocks:" <<endl;
	for ( vector<Flock *>::const_iterator itf = flocks.begin(); itf != flocks.end(); ++itf)
	{
		cout << "Flock #" << (*itf)->getSN() << ":" <<endl;
		for ( Flock::const_iterator itp = (*itf)->begin(); itp != (*itf)->end(); ++itp)
		{
			(*itp)->print();
		}
	}
	cout << "=== UfoDB Pilots:" <<endl;
	for ( vector<Pilot *>::const_iterator it = pilots.begin(); it != pilots.end(); ++it)
	{
		(*it)->print();
	}
	cout << "===END UfoDB" <<endl;
}

const std::string ufo::UfoDB::findPluginFile(const std::string name) const
{
	for (vector<string>::const_iterator it = pluginDirectories.begin();
			it != pluginDirectories.end(); ++it)
	{
		string filename =  (*it) + "/" + name + ".so";
		PRINT("Trying plugin file %s\n",filename.c_str());
		// could we read the file if we tried?
		if ( access(filename.c_str(), R_OK ) == 0) {
			return filename;
		}
	}
	PRINT("Path lookup for plugin file \"%s\" failed.\n",name.c_str());
	return "";
}
