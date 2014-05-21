#include "./UtilityFunctions.h"
#include "./Configuration.h"

// This software is distributed under the terms of the GNU Library General Public License version 2 or later as published by the Free Software Foundation.

namespace
{

	std::string getConfiguredPath(const std::string& configurationPathKey)
	{
		return Configuration::getPath(configurationPathKey);
	}
}

std::string getConcatenatedPath(const std::string& path, const std::string& configurationPathKey)
{
	return getConfiguredPath(configurationPathKey) += path;
}

std::vector<std::string> tokenizeString(const std::string& theString, const std::string &delimiters)
{
	std::vector< std::string > result;
	std::string::size_type tokenBegin, tokenEnd;
	tokenEnd = 0;
	do {
		// skip delimiter:
		tokenBegin = theString.find_first_not_of(delimiters, tokenEnd);
		tokenEnd = theString.find_first_of(delimiters,tokenBegin);
		// add partial string to result:
		if ( tokenBegin < theString.size() )
		{
			result.push_back(theString.substr(tokenBegin,tokenEnd - tokenBegin));
		}
	} while ( tokenEnd != std::string::npos );
	return result;
}
