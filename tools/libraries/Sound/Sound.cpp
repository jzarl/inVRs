#include <gmtl/VecOps.h>
#include "Sound.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

ALCdevice* Sound::device = NULL;
ALCcontext* Sound::context = NULL;
gmtl::Vec3f Sound::posListener = gmtl::Vec3f(0, 0, 0);
gmtl::Vec3f Sound::upListener = gmtl::Vec3f(0, 1, 0);
gmtl::Vec3f Sound::forwardListener = gmtl::Vec3f(0, 0, 1);
gmtl::Vec3f Sound::velocityListener = gmtl::Vec3f(0, 0, 0);

#ifndef MIN2
#define MIN2(x,y)		((x)<(y)?(x):(y))
#endif

//define this via -DUSE_ALCCHAR in /make/config*.mk:
//#define USE_ALCCHAR
// if you really want to use the deprecated alutLoadWAVFile, uncomment this:
//#define ALUT_USE_DEPRECATED_FUNCTIONS

Sound::Sound()
{
	bInitialized = false;
	bPlaying = false;
	if(device == NULL) staticInit();
	sndBuffer = 0;
	sndSource = 0;
	gain = 1.0f;
	pos = gmtl::Vec3f(0, 0, 0);
	velocity = gmtl::Vec3f(0, 0, 0);
	maxDistance = 100;
	rolloff = 1.0f;
	pitch = 1.0f;
	emulDist = 0;
	emulDistOff = 0.5;
	emulDistScale = 16;
	emulDopplerFactor = 1;
	emulSpeedOfSound = 500;
	posRelToListener = false;
}

Sound::~Sound()
{

}

void Sound::release()
{
	if(bPlaying) this->stop();
	if(sndSource) alDeleteSources(1, &sndSource);
	if(sndBuffer) alDeleteBuffers(1, &sndBuffer);
}

bool Sound::init(std::string file)
{
	ALint nChannels = 0;
	FILE* testFile;

	if(!device || !context)
	{
		printd(ERROR, "Sound::init(): sound initialization failed!\n");
		return false;
	}

	alGetError();

	// 2013-10-14 ZaJ: is this really neccessary?
	testFile = fopen(file.c_str(), "rb");
	if(!testFile)
	{
		printd(ERROR, "Sound::init(): cannot read from file %s, does it exist???\n", file.c_str());
		return false;
	}
	fclose(testFile);

#ifndef ALUT_USE_DEPRECATED_FUNCTIONS
	// if we ever need to read files in another format,
	// have a look here: http://kcat.strangesoft.net/openal-tutorial.html
	// alternatively, we could also look into using ALURE:
	// http://kcat.strangesoft.net/alure.html
	ALint bits=0;
	sndBuffer = alutCreateBufferFromFile(file.c_str());
	if ( ALenum err=alutGetError() != ALUT_ERROR_NO_ERROR )
	{
		printd(ERROR, "Sound::init(): error creating buffer for %s\n", file.c_str());
		printd(DEBUG, "Error message: %s\n", alutGetErrorString(err));
		sndBuffer = 0;
		return false;
	}
	// query buffer info:
	alGetBufferi(sndBuffer, AL_SIZE, &size);
	alGetBufferi(sndBuffer, AL_BITS, &bits);
	alGetBufferi(sndBuffer, AL_FREQUENCY, &freq);
	alGetBufferi(sndBuffer, AL_CHANNELS, &nChannels);
	if ( bits == 8 )
		format = (nChannels==1)? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
	else if ( bits == 16 )
		format = (nChannels==1)? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	else // should not be possible:
		printd(WARNING, "Sound::init(): unusual bit setting of %u in file %s. Audio sample will probably not work.\n", bits, file.c_str());
#else
	ALboolean loop;
	ALvoid* data;

	alutLoadWAVFile((ALbyte*)file.c_str(), &format, &data, &size, &freq, &loop);
	if(alGetError() != AL_NO_ERROR)
	{
		printd(ERROR, "Sound::init(): failed to load audio data from file %s\n", file.c_str());
		return false;
	}

	alGenBuffers(1, &sndBuffer);
	if(alGetError() != AL_NO_ERROR)
	{
		printd(ERROR, "Sound::init(): error creating buffer for %s\n", file.c_str());
		sndBuffer = 0;
		return false;
	}
	assert(sndBuffer != 0);

	alBufferData(sndBuffer, format, data, size, freq);

	alutUnloadWAV(format, data, size, freq);
	// query missing buffer info:
	alGetBufferi(sndBuffer, AL_CHANNELS, &nChannels);

	alGetError();
#endif
	assert(sndBuffer != 0 && alIsBuffer(sndBuffer) == AL_TRUE);

	printd(INFO, "Sound::init(): %s: format: %u, size: %u, channels %u\n", file.c_str(), format, size, nChannels);

	alGenSources(1, &sndSource);

	if(alGetError() != AL_NO_ERROR)
	{
		printd(ERROR, "Sound::init(): error creating source for %s\n", file.c_str());
		alDeleteBuffers(1, &sndBuffer);
		sndSource = 0;
		sndBuffer = 0;
		return false;
	}
	assert(sndSource != 0);

	bInitialized = true;
	return true;
}

bool Sound::staticInit()
{
	char* deviceName = NULL;
	char* deviceList = NULL;

	#if defined USE_ALCCHAR
		ALCchar* enumstr = (ALCchar*)"ALC_ENUMERATION_EXT";
		ALchar* devnamestr;
	#else
		ALubyte* enumstr = (ALubyte*)"ALC_ENUMERATION_EXT";
		ALubyte* devnamestr;
	#endif

	printd(INFO, "Sound::staticInit(): OpenAL:\nVersion: %s\nRenderer: %s\nVendor: %s\nExtensions: %s\n", (char*)alGetString(AL_VERSION), (char*)alGetString(AL_RENDERER), (char*)alGetString(AL_VENDOR), (char*)alGetString(AL_EXTENSIONS));

	if(alcIsExtensionPresent(NULL, enumstr) == AL_TRUE)
	{
		deviceList = (char*)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		if(deviceList)
		{
			printd(INFO, "sound devices:\n");
			while(true)
			{
				printd(INFO, "%s\n", deviceList);
				if(strstr(deviceList, "ardware"))
				{
					deviceName = deviceList;
				}
				deviceList += strlen(deviceList);
				if (deviceList[0] == 0) {
					if (deviceList[1] == 0) {
						break;
					} else {
						deviceList += 1;
					}
				}
			}
		}
	}

	#if defined USE_ALCCHAR
		devnamestr = (ALchar*)deviceName;
	#else
		devnamestr = (ALubyte*)deviceName;
	#endif

	printd(INFO, "opening %s - device\n", deviceName ? deviceName : "default");
	device = alcOpenDevice(devnamestr);
//	if(deviceName) delete [] deviceName;

	if(device == NULL)
	{
		printd(ERROR, "Sound::staticInit(): failed to initialize OpenAL\n");
		return false;
	}
	context=alcCreateContext(device, NULL);
	if(context == NULL)
	{
		printd(ERROR, "Sound::staticInit(): failed to initialize OpenAL context\n");
		cleanup();
		return false;
	}
	alcGetError(device);
	alcMakeContextCurrent(context);
	if(alcGetError(device) != ALC_NO_ERROR)
	{
		printd(ERROR, "Sound::staticInit(): failed to make context current\n");
		cleanup();
		return false;
	}

	alGetError();
	alDistanceModel(AL_NONE);
	alDopplerFactor(4);
	alDopplerVelocity(10);
	//alSpeedOfSound(10);
	if(alGetError() != AL_NO_ERROR)
	{
		printd(ERROR, "Sound::staticInit(): failed to set distance model\n");
		cleanup();
		return false;
	}

	alcProcessContext(context);

	// we manage the AL context without ALUT, so we just let ALUT do its internal init.
	// this is needed so that other ALUT functions don't bail out on us.
	if ( alutInitWithoutContext(NULL,NULL) != AL_TRUE )
	{
		if ( alutGetError() == ALUT_ERROR_INVALID_OPERATION )
		{
			printd(ERROR, "Sound::staticInit(): failed to initialize ALUT: already initialized\n");
		} else {
			printd(ERROR, "Sound::staticInit(): failed to initialize ALUT: unknown error\n");
		}
		cleanup();
		return false;
	}

	return true;
}

void Sound::cleanup(void)
{
	// alutExit shouldn't be really needed, since we don't manage the context via ALUT.
	// we do it anyways, just to be on the safe side:
	alutExit();
	alcMakeContextCurrent(NULL);
	if(context) alcDestroyContext(context);
	if(device) alcCloseDevice(device);
	device = NULL;
	context = NULL;
}

void Sound::setDistanceModel(ALenum model)
{
	if(!device || !context)
	{
// 		printf("Sound::setDistanceModel(): context and/or device not set\n");
		return;
	}
	alDistanceModel(model);
}

void Sound::play(bool bRepeat)
{
	ALfloat myPos[3], myVel[3];
	ALenum err;
	int i;

	if(!device || !context)
	{
// 		printf("Sound::play(): context and/or device not set\n");
		return;
	}
	if(!bInitialized)
	{
		printd(ERROR, "Sound::play(): *this not initialized\n");
		return;
	}

	for(i=0;i<3;i++)
	{
		myPos[i] = (ALfloat)pos[i];
		myVel[i] = (ALfloat)velocity[i];
	}

	ALint value;
	alGetSourcei(sndSource, AL_SOURCE_STATE, &value);
	if(bPlaying && (value == AL_PLAYING)) return;

	alGetError();
	alSourcei(sndSource, AL_SOURCE_RELATIVE, AL_FALSE);
	alSourcef(sndSource, AL_PITCH, pitch);
	alSourcef(sndSource, AL_GAIN, gain);
	alSourcef(sndSource, AL_MAX_GAIN, gain);
	alSourcef(sndSource, AL_MIN_GAIN, 0.0f);
	alSourcef(sndSource, AL_MAX_DISTANCE, maxDistance);
	alSourcef(sndSource, AL_REFERENCE_DISTANCE, maxDistance/2.0f);
	alSourcef(sndSource, AL_ROLLOFF_FACTOR, rolloff);
	alSourcefv(sndSource, AL_POSITION, myPos);
	alSourcefv(sndSource, AL_VELOCITY, myVel);
	alSourcei(sndSource, AL_BUFFER, sndBuffer);
	alSourcei(sndSource, AL_LOOPING, bRepeat ? AL_TRUE : AL_FALSE);
	alSourcei(sndSource, AL_SOURCE_RELATIVE, posRelToListener ? AL_TRUE : AL_FALSE);
	alSourceRewind(sndSource);
	alSourceRewind(sndSource);
	err = alGetError();
	if(err != AL_NO_ERROR)
	{
		printd(ERROR, "Sound::play(): play setup failed %d\n", err);
	}
	alSourcePlay(sndSource);
	printd(INFO, "Sound::play(): started playing ...\n");

	bPlaying = true;
}

void Sound::setRelativeSource(bool posRelToListener)
{
	this->posRelToListener = posRelToListener;
	if(bPlaying)
	{
		alSourcei(sndSource, AL_SOURCE_RELATIVE, posRelToListener ? AL_TRUE : AL_FALSE);
	}
}

void Sound::stop()
{
	if(!device || !context)
	{
// 		printf("Sound::stop(): context and/or device not set\n");
		return;
	}
	if(!bInitialized)
	{
		printd(ERROR, "Sound::stop(): *this not initialized\n");
		return;
	}
	if(!bPlaying) return;
	alSourceStop(sndSource);
}

void Sound::setListener(gmtl::Vec3f pos, gmtl::Vec3f up, gmtl::Vec3f forward, gmtl::Vec3f velocity)
{
	ALfloat listenerOri[6];
	ALfloat listenerPos[3];
	ALfloat listenerVel[3];
	int i;

	if(!device || !context)
	{
// 		printf("Sound::setListener(): context and/or device not set\n");
		return;
	}

	posListener = pos;
	upListener = up;
	forwardListener = forward;
	velocityListener = velocity;

	for(i=0;i<3;i++)
	{
		listenerOri[i] = (ALfloat)forwardListener[i];
		listenerOri[3+i] = (ALfloat)upListener[i];
		listenerPos[i] = (ALfloat)posListener[i];
		listenerVel[i] = (ALfloat)velocityListener[i];
	}

	alGetError();
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	if(alGetError()!=AL_NO_ERROR)
	{
		printd(ERROR, "Sound::setListener(): failed to set listener!\n");
	}
}

void Sound::setPos(gmtl::Vec3f pos)
{
	this->pos = pos;
	if(bPlaying)
	{
		alSource3f(sndSource, AL_POSITION, (ALfloat)pos[0], (ALfloat)pos[1], (ALfloat)pos[2]);
	}
}

gmtl::Vec3f Sound::getPos()
{
	return pos;
}

void Sound::setVelocity(gmtl::Vec3f vel)
{
	this->velocity = vel;
	if(bPlaying)
	{
		alSource3f(sndSource, AL_VELOCITY, (ALfloat)vel[0], (ALfloat)vel[1], (ALfloat)vel[2]);
	}
}

void Sound::setGain(float value)
{
	gain = value;
	if(bPlaying)
	{
		alSourcef(sndSource, AL_GAIN, gain);
		alSourcef(sndSource, AL_MAX_GAIN, gain);
		alSourcef(sndSource, AL_MIN_GAIN, 0.1f);
	}
}

void Sound::setMaxDistance(float value)
{
	maxDistance = value;
	if(bPlaying)
	{
		alSourcef(sndSource, AL_MAX_DISTANCE, maxDistance);
		alSourcef(sndSource, AL_REFERENCE_DISTANCE, maxDistance/2);
	}
}

void Sound::setRolloffFactor(float value)
{
	rolloff = value;
	if(bPlaying)
	{
		alSourcef(sndSource, AL_ROLLOFF_FACTOR, rolloff);
	}
}

void Sound::setPitch(float pitch)
{
	this->pitch = pitch;
	if(bPlaying)
	{
		alSourcef(sndSource, AL_PITCH, pitch);
	}
}

void Sound::setupDistanceAttenuationEmulation(float scale, float off)
{
	emulDistScale = (ALfloat)scale;
	emulDistOff = (ALfloat)off;
}

void Sound::setDistEmul(float dist)
{
	emulDist = (ALfloat)dist;
	float newgain;
	if(bPlaying)
	{
		newgain = gain*(emulDistScale/(emulDist + emulDistOff));
		alSourcef(sndSource, AL_GAIN, newgain);
// 		std::cerr << "Sound::setDistEmul(): setting gain to " << newgain << "\n";
	}
}

void Sound::setDistEmulOpenaALInverseDistance(float dist)
{
	emulDist = (ALfloat)dist;
	float refDist;
	float newgain;
	if(bPlaying)
	{
// 		if(dist>maxDistance) alSourcef(sndSource, AL_GAIN, 0);
// 		else
		{
			refDist = maxDistance*0.5;
			newgain = gain*(refDist/(refDist + rolloff*(dist-refDist)));
			alSourcef(sndSource, AL_GAIN, newgain);
// 			std::cerr << "Sound::setDistEmulOpenaALInverseDistance(): setting gain to " << newgain << "\n";
		}
	}
}

void Sound::setupDopplerShift(float speedOfSound, float factor)
{
	this->emulSpeedOfSound = (ALfloat)speedOfSound;
	this->emulDopplerFactor = (ALfloat)factor;
}

void Sound::setDopplerShift(gmtl::Vec3f velList, gmtl::Vec3f posListner, gmtl::Vec3f velSrc, gmtl::Vec3f posSrc)
{
	if(!bPlaying) return;
	gmtl::Vec3f dSrcList = posListner - posSrc;
	ALfloat vl, vs, newPitch, vls, vss;
	vl = (ALfloat)(dot(dSrcList, velList)/length(dSrcList));
	vs = (ALfloat)(dot(dSrcList, velSrc)/length(dSrcList));
	vss = MIN2(vs, emulSpeedOfSound/emulDopplerFactor);
	vls = MIN2(vl, emulSpeedOfSound/emulDopplerFactor);
	newPitch = pitch*((emulSpeedOfSound-emulDopplerFactor*vls)/(emulSpeedOfSound-emulDopplerFactor*vss));
	alSourcef(sndSource, AL_PITCH, newPitch);
}

void Sound::resetDopplerShift()
{
	if(!bPlaying) return;
	alSourcef(sndSource, AL_PITCH, pitch);
}

