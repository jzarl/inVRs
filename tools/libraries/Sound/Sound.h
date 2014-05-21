#ifndef _SOUND_H
#define _SOUND_H

#include <gmtl/Generate.h>
#include <gmtl/VecOps.h>
// #include <OpenSG/OSGVector.h>
// #include <OpenSG/OSGMatrix.h>
// #include <OpenSG/OSGQuaternion.h>
// #include <OpenSG/OSGBaseTypes.h>
#include <string>
#ifdef WIN32
	#include <windows.h>
	#include <conio.h>
	#include <alc.h>
	#include <alut.h>
	#include <al.h>
#else
  #if defined(MACOS_X) || defined(__APPLE_CC__)
	#include <AL/alc.h>
	#include <AL/alut.h>
	#include <AL/al.h>
  #else
  	#include <AL/alc.h>
	#include <AL/alut.h>
	#include <AL/al.h>
  #endif
#endif

// #define USE_ALCCHAR


/** 
 * // TODO: check if this works:
 * If the 3D-sound seems to work not properly, try to call
 * alcOpenDevice with string "Generic Software"
 *
 */

class Sound
{
	public:

	Sound();
	~Sound();

	bool init(std::string file);
	void release();

	void play(bool bRepeat = true);
	void stop();
	void setPos(gmtl::Vec3f pos);
	void setPitch(float pitch);
	void setVelocity(gmtl::Vec3f vel);
	void setGain(float value);
	void setMaxDistance(float value);
	void setRolloffFactor(float value);
	void setRelativeSource(bool posRelToListener);
	gmtl::Vec3f getPos();

	void setupDistanceAttenuationEmulation(float scale, float off);
	void setDistEmul(float dist);
	void setDistEmulOpenaALInverseDistance(float dist);
	void setupDopplerShift(float speedOfSound, float factor);
	void setDopplerShift(gmtl::Vec3f velList, gmtl::Vec3f posListner, gmtl::Vec3f velSrc, gmtl::Vec3f posSrc);
	void resetDopplerShift();

	static void cleanup();
	static void setListener(gmtl::Vec3f pos, gmtl::Vec3f up, gmtl::Vec3f forward, gmtl::Vec3f velocity);
	static void setDistanceModel(ALenum model);

	protected:

	gmtl::Vec3f pos;
	gmtl::Vec3f velocity;
	ALuint sndBuffer;
	ALuint sndSource;
	bool bInitialized;
	bool bPlaying;
	bool posRelToListener;
	ALsizei size;
	ALsizei freq;
	ALenum format;
	ALfloat gain;
	ALfloat maxDistance;
	ALfloat rolloff;
	ALfloat pitch;
	
	ALfloat emulDistScale;
	ALfloat emulDistOff;
	ALfloat emulDist;
	ALfloat emulSpeedOfSound;
	ALfloat emulDopplerFactor;

	static ALCdevice *device;
	static ALCcontext *context;
	static gmtl::Vec3f posListener;
	static gmtl::Vec3f upListener;
	static gmtl::Vec3f forwardListener;
	static gmtl::Vec3f velocityListener;

	static bool staticInit();

};

#endif
