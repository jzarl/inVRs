#include <cstdio>

#include <ufo/UfoDB.h>
#include <ufo/Configurator.h>
#include <ufo/ConfigurationReader.h>
#include <ufo/PlainConfigurationReader.h>

using namespace ufo;

/**
 * This is the same as 03testpluginloat, but uses a different configuration
 */
int main(int argc, char **argv)
{
	Configurator *cfg;
	ConfigurationReader *cfgReader;
	Flock *f;
	Pilot *p;
	
	if ( argc == 2 )
	{
		cfgReader = new PlainConfigurationReader(argv[1]);
	} else {
		cfgReader = new PlainConfigurationReader("test/06testbehaviours.cfg");
	}

	cfg = cfgReader->readConfig();

	// we don't need the cfgReader any more:
	delete cfgReader;

	if ( cfg == 0 )
	{
		printf("ERROR: readConfig() failed!\n");
		return 1;
	}

	// tell ufo where to search
	UfoDB::the()->addPluginDirectory("./ufoplugins");

	if ( ! cfg->bind() )
	{
		printf("ERROR: bind() failed!\n");
		return 1;
	}
	// we have our pilot and flock, so we don't need the configurator any more:
	delete cfg;

	printf("INFO: configuration has %d Flocks and %d independent Pilots.\n"
			, UfoDB::the()->getFlocks().size(), UfoDB::the()->getPilots().size());
	f = *(UfoDB::the()->getFlocks().begin());
	p = *(UfoDB::the()->getPilots().begin());
	printf("INFO: first flock has %d Pilots. \n", f->size());

	//printf("INFO: pilot->print()\n");
	//p->print();


	// let's test updating the flock and pilot:
	for (int i=0; i<3000; i++)
	{
		f->update(0.1f);
		p->steer(0.1f);
	}
	printf("INFO: final position of p = (%2.2f,%2.2f,%2.2f)\n"
			,p->getPosition()[0]
			,p->getPosition()[1]
			,p->getPosition()[2] );

	UfoDB::reset();
	return 0;
}
