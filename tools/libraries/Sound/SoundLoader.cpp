#include "SoundLoader.h"
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <stdio.h>
#include <string.h>

SoundLoader::SoundLoader()
{

}

SoundLoader::~SoundLoader()
{
	int i;
	KEY_VALUE pair;

	for(i=0;i<(int)loadedPairs.size();i++)
	{
		pair = loadedPairs.at(i);
		delete [] pair.name;
		delete [] pair.path;
	}
}

void SoundLoader::loadConfig(std::string configFile)
{
// 	char nameBuffer[2048];
// 	char pathBuffer[2048];
// 	KEY_VALUE pair;
// 	FILE* f = fopen(path, "r");
// 	int error, soundpathLen;
// 	std::string soundpath = Configuration::getSoundPath();
// 	if(!f)
// 	{
// 		printf("Soundloader::loadConfigFile(): error loading file %s\n", path);
// 		return;
// 	}
// 	soundpathLen = (int)strlen(soundpath.c_str());
// 	while(true)
// 	{
// 		error = fscanf(f, "%s = %s\n", nameBuffer, pathBuffer);
// 		if(error != EOF)
// 		{
// 			pair.name = new char[strlen(nameBuffer)+1];
// 			pair.path = new char[strlen(pathBuffer)+1+soundpathLen];
// 			strcpy(pair.path, soundpath.c_str());
// 			strcpy(pair.name, nameBuffer);
// 			strcat(pair.path, pathBuffer);
// 			loadedPairs.push_back(pair);
// 			printf("Soundloader::loadConfigFile(): bind %s to %s\n", pair.name, pair.path);
// 		} else break;
// 	}
// 	fclose(f);

	IrrXMLReader* xml;
	std::string name;
	std::string fileName;
	std::string fullPath;
	KEY_VALUE pair;

	configFile = Configuration::getPath("SoundConfiguration")+configFile;

	if (!fileExists(configFile))
	{
		printd(ERROR, "SoundLoader::loadConfig(): ERROR: could not find config-file %s\n", configFile.c_str());
		return;
	} // if

	xml = createIrrXMLReader(configFile.c_str());

	while(xml && xml->read()){
		switch (xml -> getNodeType()){
			case EXN_TEXT:
				break;
			case EXN_ELEMENT:
				if(!strcmp("sound", xml->getNodeName()))
				{
					name = xml->getAttributeValue("name");
					fileName = xml->getAttributeValue("file");
					fullPath = Configuration::getPath("Sounds") + fileName;
					pair.name = new char[name.size()+1];
					pair.path = new char[fullPath.size()+1];
					strcpy(pair.name, name.c_str());
					strcpy(pair.path, fullPath.c_str());
					printd(INFO, "SoundLoader::loadConfig(): setting file of %s to %s\n", pair.name, pair.path);
					loadedPairs.push_back(pair);
				}
				break;
			case EXN_ELEMENT_END:
				break;
			default:
				break;
		} // switch
	} // while
	delete xml;
}

bool SoundLoader::initSound(Sound* sound, std::string name)
{
	int i;
	KEY_VALUE pair;

	for(i=0;i<(int)loadedPairs.size();i++)
	{
		pair = loadedPairs.at(i);
		if(!strcmp(pair.name, name.c_str()))
			return sound->init(pair.path);
	}
	printd(ERROR, "SoundLoader::initSound(): couldn't find a matching sound for %s\n", name.c_str());
	return false;
}

