#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <ufo/UfoDB.h>
#include <ufo/Configurator.h>
#include <ufo/ConfigurationReader.h>
#include <ufo/PlainConfigurationReader.h>

using namespace ufo;

#define NUMPARALLEL 10
#define NUMSEQUENTIAL 10
/**
 * This is the same as 03testpluginload, but does run several times.
 * This may uncover blatant memory leaks (or not).
 */
int runtest(char *cfgfile)
{
	Configurator *cfg;
	ConfigurationReader *cfgReader;
	Flock *f;
	Pilot *p;

	if ( cfgfile )
	{
		cfgReader = new PlainConfigurationReader(cfgfile);
	} else {
		cfgReader = new PlainConfigurationReader("test/04testpluginmultiload.cfg");
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
	// we have our Flocks and Pilots, so we don't need the Configurator anymore:
	delete cfg;

	f = *(UfoDB::the()->getFlocks().begin());
	p = *(UfoDB::the()->getPilots().begin());


	// let's test updating the flock and pilot:
	for (int i=0; i<3000; i++)
	{
		f->update(0.1f);
		p->steer(0.1f);
	}

	UfoDB::reset();
	return 0;
}

int forktest(char *cfgfile)
{
	int pid;
	pid  = fork();
	switch ( pid )
	{
		case -1: /* fork failed */
			perror("fork failed!");
			return 0;
			break;
		case 0: /* this is the forked process */
			for ( int i=0; i < NUMSEQUENTIAL; i++)
			{
				if ( runtest(cfgfile) != 0 )
				{
					printf("Test run #%d failed!\n",i);
					exit(1);
				}
			}
			exit(0);
		default: /* this is the parent process (fork() returned pid of child) */
			break;
	}
	return pid;
}


int main(int argc, char **argv)
{
	int i;
	int retval = 0;
	
	char *cfgfile = 0;
	if ( argc == 2 )
	{
		cfgfile = argv[1];
	}
	printf("Creating %d threads with %d sequential threads each.\n",NUMPARALLEL,NUMSEQUENTIAL);
	for ( i=0; i<NUMPARALLEL; i++)
	{
		int pid = forktest(cfgfile);
		if ( pid == 0 )
		{
			printf("Fork #%d failed!\n",i);
			retval++;
			break;
		} else {
			//printf("Fork #%d created pid %d\n",i,pid);
		}
	}
	i--;
	// wait for as many child processes as we created:
	for ( ; i>=0; i--)
	{
		int status;
		//printf("Waiting for child #%d\n",i);
		wait(&status);
		if ( WIFEXITED(status) )
			if ( WEXITSTATUS(status) != 0 )
			{
				retval++;
				printf("Child exited with status %d\n",WEXITSTATUS(status));
			}
		if ( WIFSIGNALED(status))
		{
			retval++;
			printf("Child was terminated by signal %d!\n",WTERMSIG(status));
		}
	}
	return retval;
}
