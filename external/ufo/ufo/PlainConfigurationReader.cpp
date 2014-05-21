#include "PlainConfigurationReader.h"

#include <cstdlib> // for getenv
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept> // std::runtime_error

// override global debug flag:
#undef DEBUG
#include "Debug.h"
#include "Configurator.h"
#include "ConfigurationElement.h"

using namespace std;
using namespace ufo;

	ufo::PlainConfigurationReader::PlainConfigurationReader( const string filename )
: filename(filename),cfgfile(*(new ifstream( filename.c_str() )))
{
	PRINT("Loading plain configuration from file \"%s\".", filename.c_str());

	if ( ! ( cfgfile.is_open() && cfgfile.good()) )
	{
		string errortext;
		errortext = "ERROR: Couldn't open file \"" + filename + "\"";
		cout << errortext << endl;
		throw runtime_error(errortext);
	}
}

ufo::PlainConfigurationReader::~PlainConfigurationReader()
{
	cfgfile.close();
	// yes, I really mean this (look at the constructor):
	delete &cfgfile;
}

ufo::Configurator * ufo::PlainConfigurationReader::readConfig()
{
	Configurator *cfg = new Configurator();
	//load configuration:
	try 
	{
		string tmp;
		cfgfile >> tmp;
		if ( tmp != "UFOCFGPLAIN" )
		{
			cout << "ERROR: File \"" << filename << "\" is not an UFO plain configuration file." << endl;
			delete cfg;
			return 0;
		}

		cfgfile >> tmp;
		while ( tmp == "pluginDirectory" || tmp == "immediate" || tmp == "fromTemplate" || tmp == "template" || tmp == "/*" )
		{
			PRINT("Root Element: %s\n",tmp.c_str());
			if ( tmp == "pluginDirectory" )
			{
				string path = readString();
				// check if path is relative:
				if ( path[0] != '/' )
				{
						path = getConfigDirectory() + "/" + path;
				}
				PRINT("INFO: readConfig(): adding Plugin directory %s\n",path.c_str());
				ConfigurationReader::addPluginDir(cfg, path);
			} else if ( tmp == "immediate" )
			{
				ConfigurationElement *elem = readElement(false);
				if ( elem == 0 )
				{
					PRINT("immediate null\n");
					delete cfg;
					return 0;
				}
				ConfigurationReader::addElement(cfg, elem);
			} else if ( tmp == "fromTemplate" )
			{
				ConfigurationElement *elem = readElement(true);
				if ( elem == 0 )
				{
					PRINT("fromTemplate null\n");
					delete cfg;
					return 0;
				}
				ConfigurationReader::addElement(cfg, elem);
			} else if ( tmp == "template" )
			{
				string tplName;
				cfgfile >> tplName;
				ConfigurationElement *elem = readElement(false);
				if ( elem == 0 )
				{
					PRINT("template null\n");
					delete cfg;
					return 0;
				}
				ConfigurationReader::setTemplate(cfg,tplName,elem);
			} else // only "/*" left
			{
				skipComment();
			}
			// on eof the read fails, leaving tmp as it were. Thus we have to clear it:
			tmp.clear();
			cfgfile >> tmp;
		}
		PRINT("All elements read.\n");
	} 
	catch ( ios_base::failure &f )
	{
		cout << "ERROR: IO-error while reading file \"" <<filename<<"\": " <<f.what() <<endl;
		delete cfg;
		return 0;
	}
	return cfg;
}

ufo::ConfigurationElement * ufo::PlainConfigurationReader::readElement( bool fromTemplate )
{
	ConfigurationElement *elem;
	string tmp;
	// read type:
	cfgfile >> tmp;
	PRINT("Element type: %s(%s)\n",tmp.c_str(),fromTemplate?"fromTemplate":"immediate");
	if ( tmp == "FLOCK" )
	{
		elem = new ConfigurationElement(ConfigurationElement::FLOCK, fromTemplate);
	} else if ( tmp == "PILOT" )
	{
		elem = new ConfigurationElement(ConfigurationElement::PILOT, fromTemplate);
	} else if ( tmp == "BEHAVIOUR" )
	{
		elem = new ConfigurationElement(ConfigurationElement::BEHAVIOUR, fromTemplate);
	} else if ( tmp == "STEERABLE" )
	{
		elem = new ConfigurationElement(ConfigurationElement::STEERABLE, fromTemplate);
	} else
	{
		cout << "ERROR: illegal element type \"" << tmp << "\" in place of (FLOCK|PILOT|BEHAVIOUR|STEERABLE)!" << endl;
		return 0;
	}
	// read name:
	cfgfile >> tmp;
	elem->name = tmp;
	PRINT("Name: %s\n",tmp.c_str());

	skip("{");

	// read parameters and children
	cfgfile >> tmp;
	while ( tmp != "}" )
	{
		if ( tmp == "parameters" )
		{
			skip("{");
			// read parameters
			cfgfile >> tmp;
			while ( tmp != "}" )
			{
				if ( tmp == "/*" )
				{
					skipComment();
					cfgfile >> tmp;
				}
				string key,val;
				// save key
				key = tmp;
				PRINT("KEY: %s\n",key.c_str());
				skip("=");
				// read value
				val = readString();
				PRINT("VAL: %s\n",val.c_str());
				elem->parameters.push_back(pair<string,string>(key,val));
				cfgfile >> tmp;
			}
		} else if ( tmp == "children" )
		{
			skip("{");
			// read children and skip "}"
			cfgfile >> tmp;
			while ( tmp != "}" )
			{
				if ( tmp == "/*" )
				{
					skipComment();
					cfgfile >> tmp;
				}
				PRINT("Element: %s\n",tmp.c_str());
				if ( tmp == "immediate" )
				{
					ConfigurationElement *child = readElement(false);
					if ( child == 0 )
					{
						PRINT("immediate child null\n");
						delete elem;
						delete child;
						return 0;
					}
					elem->children.push_back(child);
				} else if ( tmp == "fromTemplate" )
				{
					ConfigurationElement *child = readElement(true);
					if ( child == 0 )
					{
						PRINT("fromTemplate child null\n");
						delete elem;
						delete child;
						return 0;
					}
					elem->children.push_back(child);
				} else if ( tmp == "template" )
				{
					cout << "WARNING: template element cannot be child of another element (ignoring template)!" <<endl;
					readElement(false);
				} else {
					cout << "ERROR: Unexpected token \"" << tmp << "\" in place of (immediate|fromTemplate|template)!" <<endl;
					delete elem;
					return 0;
				}
				cfgfile >> tmp;
			}
		} else if ( tmp == "/*" )
		{
			skipComment();
		} else
		{
			cout << "ERROR: unexpected token \"" << tmp << "\" in place of (children|parameters)!" << endl;
			delete elem;
			return 0;
		}
		cfgfile >> tmp;
		// tmp == (children|parameters|'}')
		// i.e. this already skips the closing bracket of the element
	}
	PRINT("Successfully read Element %s\n", elem->name.c_str());
	return elem;
}

std::string ufo::PlainConfigurationReader::readString()
{
	string val,tmp;
	// value may be one word or a string
	cfgfile >> val;
	if ( *val.begin() == '"' )
	{
		while ( *val.rbegin() != '"' ){
			cfgfile >> tmp;
			val.append(" ");
			val.append(tmp);
		}
		// erase quotes from begin and end of string:
		val.erase(0,1);
		val.erase(val.size()-1,1);
	} 
	return val;
}

bool ufo::PlainConfigurationReader::skip(std::string token)
{
	string tmp;
	cfgfile >> tmp;
	if ( tmp != token )
	{
		cout << "WARNING: expected token \"" << token <<"\", but was \"" << tmp << "\"." << endl;
		return false;
	}
	return true;
}
bool ufo::PlainConfigurationReader::skipComment()
{
	string tmp;
	// consume everything to the comment-closing-tag
	while ( cfgfile.good() && tmp != "*/" )
	{
		cfgfile >> tmp;
		PRINT("skipping %s\n", tmp.c_str());
	}
	if ( cfgfile.good() )
	{
		return true;
	} else {
		cout << "WARNING: unexpected end of configuration durin comment." <<endl;
		return false;
	}
}
std::string ufo::PlainConfigurationReader::getConfigDirectory()
{
	size_t pos;
	string configDirectory;

	// get path of configuration file:
	// /path/to/cfg -> /path/to
	// cfg -> ""
	pos = filename.find_last_of("/"); 
	if ( pos == filename.npos ) //not found -> current wd
	{
		configDirectory = ".";
	} else {
		configDirectory = filename.substr(0,pos);
	}

	// is the path absolute?
	if ( configDirectory[0] == '/' )
	{
		// we can use it:
		return configDirectory;
	} else { // make it absolute:
		char *cwd;
		
		// this is way too ugly, but I hope it works:
		// get current working directory:
		cwd = getenv("PWD");
		if ( ! cwd )
		{
			cout << "WARNING: Couldn't get current working directory!" <<endl;
			// just pray that the application doesn't change directory:
			return "./" + configDirectory;
		}
		return cwd + ( "/" + configDirectory);
	}
}
