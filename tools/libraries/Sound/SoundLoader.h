#ifndef _SOUNDLOADER_H
#define _SOUNDLOADER_H

// #include "../../../tools/libs/include/irrXML.h"
#include "Sound.h"
#include <vector>
#include <string>


class SoundLoader
{
	public:

	SoundLoader();
	~SoundLoader();

	void loadConfig(std::string configFile);
	bool initSound(Sound* sound, std::string name);

	protected:

	class KEY_VALUE
	{
		public:
		char* name;
		char* path;
	};

	std::vector<KEY_VALUE> loadedPairs;
};

#endif
