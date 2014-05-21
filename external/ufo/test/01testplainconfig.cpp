#include <ufo/Configurator.h>
#include <ufo/ConfigurationReader.h>
#include <ufo/PlainConfigurationReader.h>
#include <cstdio>

using namespace ufo;

int main(int argc, char **argv)
{
	Configurator *cfg;
	ConfigurationReader *cfgReader;
	
	if ( argc == 2 )
	{
		cfgReader = new PlainConfigurationReader(argv[1]);
	} else {
		cfgReader = new PlainConfigurationReader("sampleconfigplain.txt");
	}

	cfg = cfgReader->readConfig();

	delete cfgReader;

	if ( cfg == 0 )
	{
		printf("ERROR: readConfig() failed!\n");
		return 1;
	}
	//cfg->print();
	printf("SUCCESS\n");

	delete cfg;
	return 0;
}
