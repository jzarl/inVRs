#include "Plugin.h"

#ifdef WIN32
/* add your windows includes here */
#else
// Some resources on the "ISO C++ forbids casting between pointer-to-function and pointer-to-object"-dilemma:
// (In case you wonder about the reinterpret_casts surrounding dlsym).
// http://groups.google.pl/group/comp.lang.c++.moderated/browse_frm/thread/51544112db2a6f1/
// http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#195
#	include <dlfcn.h> /* for dlopen, dlsym, etc. */
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Debug.h"

using namespace std;
using namespace ufo;

	ufo::Plugin::Plugin(string filename, string id, pluginType type)
: filename(filename),subtype(id),pluginhandle(0)
{
	vector<string> errors;
	char *error;

	PRINT("Loading plugin file %s\n", filename.c_str());
	pluginhandle = dlopen(filename.c_str(), RTLD_LAZY);
	if (pluginhandle == 0) 
	{
		cout << "Couldn't open plugin file" 
			<< filename << " -- maybe there are some undefined references?" << endl;
		throw runtime_error(dlerror()); 
	}

	// initialise our functions:
	// clear any previous errors:
	dlerror();

	// get symbol with name of the function:
	b_fun = reinterpret_cast<pluginBranchname_t>( 
			reinterpret_cast<size_t>(dlsym(pluginhandle, string(id + "pluginBranchname").c_str())));
	// did it work?
	error = dlerror();
	if ( error != 0 ) {
		errors.push_back(error);
	}

	// get symbol with name of the function:
	v_fun = reinterpret_cast<pluginVersion_t>( 
			reinterpret_cast<size_t>(dlsym(pluginhandle, string(id + "pluginVersion").c_str())));
	// did it work?
	error = dlerror();
	if ( error != 0 ) {
		errors.push_back(error);
	}

	// get symbol with name of the function:
	t_fun = reinterpret_cast<pluginTypename_t>( 
			reinterpret_cast<size_t>(dlsym(pluginhandle, string(id + "pluginTypename").c_str())));
	// did it work?
	error = dlerror();
	if ( error != 0 ) {
		errors.push_back(error);
	}

	// get symbol with name of the function:
	r_fun = reinterpret_cast<pluginRegister_t>( 
			reinterpret_cast<size_t>(dlsym(pluginhandle, string( id + "pluginRegister").c_str())));
	// did it work?
	error = dlerror();
	if ( error != 0 ) {
		errors.push_back(error);
	}

	// report all symbol-related errors together:
	if ( errors.size() > 0 )
	{
		string errortext;
		// close library, don't do error checking on close:
		dlclose(pluginhandle);
		cout << "The following errors occured when loading the plugin " << filename << ":" << endl;
		for ( vector<string>::const_iterator it=errors.begin(); it != errors.end(); ++it)
		{
			cout <<" * " << (*it) << endl;
			errortext += (*it) + "\n";
		}
		throw runtime_error(errortext);
	}

	// compare branch:
	if ( b_fun() != UFO_BRANCHNAME )
	{
		stringstream errortext;
		errortext << "Plugin for " << id << " was compiled for branch "
			<< b_fun() << ", but this version of libufo is branch "  UFO_BRANCHNAME  "!";
		cout << errortext << endl;
		throw runtime_error(errortext.str());
	}
	// verify the proper plugin version:
	std::string version;
	// use right version for the type of plugin:
	switch (type)
	{
		case FLOCK:
			version = UFO_FLOCK_ID;
			break;
		case PILOT:
			version = UFO_PILOT_ID;
			break;
		case BEHAVIOUR:
			version = UFO_BEHAVIOUR_ID;
			break;
		case STEERABLE:
			version = UFO_STEERABLE_ID;
			break;
		default: //cannot happen:
			throw runtime_error("Invalid plugin type encountered!");
	}
	//compare version with plugin-version:
	if ( v_fun() != version )
	{
		stringstream errortext;
		errortext << "Plugin for " << id << " has hash "
			<< v_fun() << ", but hash "
			<< version << " expected!";
		cout << errortext << endl;
		throw runtime_error(errortext.str());
	}

	// verify plugin type:
	switch (type)
	{
		case FLOCK:
			if ( t_fun() != "Flock" )
			{
				string errortext = "Plugin for " + id + " has type "
					+ t_fun() + ", but type \"Flock\" expected!";
				cout << errortext << endl;
				throw runtime_error(errortext);
			}
			break;
		case PILOT:
			if ( t_fun() != "Pilot" )
			{
				string errortext = "Plugin for " + id + " has type "
					+ t_fun() + ", but type \"Pilot\" expected!";
				cout << errortext << endl;
				throw runtime_error(errortext);
			}
			break;
		case BEHAVIOUR:
			if ( t_fun() != "Behaviour" )
			{
				string errortext = "Plugin for " + id + " has type "
					+ t_fun() + ", but type \"Behaviour\" expected!";
				cout << errortext << endl;
				throw runtime_error(errortext);
			}
			break;
		case STEERABLE:
			if ( t_fun() != "Steerable" )
			{
				string errortext = "Plugin for " + id + " has type "
					+ t_fun() + ", but type \"Steerable\" expected!";
				cout << errortext << endl;
				throw runtime_error(errortext);
			}
			break;
	}

	//register plugin:
	r_fun();
	
	PRINT("Plugin %s loaded OK\n", id.c_str());
}

ufo::Plugin::~Plugin()
{
	PRINT("Closing plugin %s\n",subtype.c_str());
	// close library:
	if ( ! dlclose(pluginhandle) )
	{
		cout << dlerror() <<endl;
	}
}

std::string ufo::Plugin::pluginFilename() const
{
	return filename;
}

std::string ufo::Plugin::pluginBranchname() const
{
	assert(b_fun != 0);
	return b_fun();
}

std::string ufo::Plugin::pluginVersion() const
{
	assert( v_fun != 0 );
	return v_fun();
}

std::string ufo::Plugin::pluginTypename() const
{
	assert(t_fun != 0);
	return t_fun();
}
