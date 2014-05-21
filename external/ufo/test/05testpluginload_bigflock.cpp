#include <cstdio>
#include <vector>

#include <ufo/UfoDB.h>
#include <ufo/Configurator.h>
#include <ufo/ConfigurationReader.h>
#include <ufo/PlainConfigurationReader.h>

using namespace ufo;
using namespace std;

/**
 * This is the same as 03testpluginload, but uses only one flock and no pilot.
 */
int main(int argc, char **argv)
{
	Configurator *cfg;
	ConfigurationReader *cfgReader;
	Flock *f;
	
	if ( argc == 2 )
	{
		cfgReader = new PlainConfigurationReader(argv[1]);
	} else {
		cfgReader = new PlainConfigurationReader("test/05testpluginload_bigflock.cfg");
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
	// we have our flock, so we don't need the configurator any more:
	delete cfg;

	printf("INFO: configuration has %d Flocks and %d independent Pilots.\n"
			, UfoDB::the()->getFlocks().size(), UfoDB::the()->getPilots().size());
	f = *(UfoDB::the()->getFlocks().begin());


	// let's test updating the flock and pilot:
	for (int i=0; i<3000; i++)
	{
		f->update(0.1f);
	}
	printf("INFO: final position of first Pilot = (%2.2f,%2.2f,%2.2f)\n"
			,f->at(0)->getPosition()[0]
			,f->at(0)->getPosition()[1]
			,f->at(0)->getPosition()[2] );

	printf("INFO: Serial numbers of Flocks:\n");
	for ( vector<Flock *>::const_iterator it = UfoDB::the()->getFlocks().begin() ;
			it != UfoDB::the()->getFlocks().end() ; ++it )
	{
		printf(" Flock SN: %u\n", (*it)->getSN() );
		for (Flock::const_iterator itp = (*it)->begin();
				itp != (*it)->end(); ++itp )
		{
			printf("  Pilot SN: %u\n", (*itp)->getSN());
		}
	}

	UfoDB::reset();
	return 0;
}
