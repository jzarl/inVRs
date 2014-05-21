#include "FactoryPlugin.h"
#include "Dso.h"

#include <stdexcept>

FactoryPlugin::FactoryPlugin( const std::string &filename, const std::string& base_type_name)
	: dso_(), base_type_name_(base_type_name)
{
	if (! dso_.open(filename))
		throw std::runtime_error(dso_.lastError());
	if (! dso_.isValid())
		throw std::runtime_error("DSO could be opened, but is not in a valid state");
	// fail early, if something's not ok:
	if ( 0 == dso_.rawSymbol("CP_NAME_" + base_type_name))
		throw std::runtime_error("DSO is not compatible to FactoryPlugin - missing symbol CP_NAME_"+base_type_name);
	if ( 0 == dso_.rawSymbol("CP_IS_COMPATIBLE"))
		throw std::runtime_error("DSO is not compatible to FactoryPlugin - missing symbol CP_IS_COMPATIBLE");
	if ( 0 == dso_.rawSymbol("CP_FACTORY"))
		throw std::runtime_error("DSO is not compatible to FactoryPlugin - missing symbol CP_FACTORY");
}

FactoryPlugin::~FactoryPlugin()
{
}

std::string FactoryPlugin::name() const
{
	const char **name = reinterpret_cast<const char **>(dso_.rawSymbol("CP_NAME_" + base_type_name_));
	return *name;
}

bool FactoryPlugin::isCompatible( const std::string &versionid )
{
	typedef bool (*compatFunction_t) (const std::string &);
	compatFunction_t compatFunction = reinterpret_cast<compatFunction_t>(dso_.rawSymbol("CP_IS_COMPATIBLE"));

	return compatFunction(versionid);
}

const Dso& FactoryPlugin::get_dso() const
{
	return dso_;
}
