#include <cstdio>

#include <ufo/UfoDB.h>
#include <ufo/Configurator.h>
#include <ufo/ConfigurationReader.h>
#include <ufo/PlainConfigurationReader.h>

#include <ufoplugins/SampleSteerable.h>
#include <ufoplugins/SampleBehaviour.h>

using namespace ufo;
using namespace ufoplugin;

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
		cfgReader = new PlainConfigurationReader("test/02testplugins.cfg");
	}

	cfg = cfgReader->readConfig();

	// we don't need the cfgReader any more:
	delete cfgReader;

	if ( cfg == 0 )
	{
		printf("ERROR: readConfig() failed!\n");
		return 1;
	}

	// as long as we don't have a plugin-runtime-loader, we have to register this manually:
	UfoDB::the()->registerSteerableType("SampleSteerable",&SampleSteerableFactory);
	UfoDB::the()->registerBehaviourType("SampleBehaviour",&SampleBehaviourFactory);

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

	// test if the flocks pilot really is different from the independent one:
	if ( f->at(0) == p )
	{
		printf("INFO: independent Pilot pointer is %p\n",static_cast<void*>(p));
		printf("ERROR: independent Pilot is the same as the Flock Pilot!\n");
		return 1;
	}

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
