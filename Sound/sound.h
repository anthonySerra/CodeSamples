//------------------------------------------------------------------------
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------
#pragma once


#include <map>
#include <list>
#include <string>
#include "fmod.hpp"
#include "..\Entity.h"

#pragma message("Knows about Sound.h")

class SOUNDVECTOR : public FMOD_VECTOR
{
public:
	SOUNDVECTOR(){x = y = z = 0;}
	SOUNDVECTOR(float _x, float _y, float _z){x=_x;y=_y;z=_z;}
	SOUNDVECTOR(float vec3[3]){x=vec3[0];y=vec3[1];z=vec3[2];}

	operator float*(){return &x;}

	const SOUNDVECTOR &operator= (const float vec3[3]){x=vec3[0];y=vec3[1];z=vec3[2];return *this;}
	SOUNDVECTOR operator+ (const float v[3]) const {return SOUNDVECTOR(x+v[0],y+v[1],z+v[2]);}

	SOUNDVECTOR operator/ (const float &f) const {return *this * (1.0f/f);}
	SOUNDVECTOR operator* (const float &f) const {return SOUNDVECTOR(x*f, y*f, z*f);}
	

	const SOUNDVECTOR &operator= (const SOUNDVECTOR& rhs){return *this = &(rhs.x);}
	SOUNDVECTOR operator+ (const SOUNDVECTOR& rhs) const {return *this + &rhs.x;}
	SOUNDVECTOR operator- (const SOUNDVECTOR& rhs) const {return SOUNDVECTOR(x-rhs.x, y-rhs.y, z-rhs.z);}
};

class SoundComponent : public BaseComponent
{
public:
	~SoundComponent() {this->channel->stop();}
	
	void Update3DAtts(const SOUNDVECTOR &pos);
  
  friend class SoundEngine;
	float GetVolume() {return Volume;}
	void SetVolume(float vol) {Volume = vol;}

	float GetSoundDuration();

	void Pause();
	void Unpause();
	bool Paused();

	void Stop();

	void SetFrequencyMultiplier(float multiplier){channel->setFrequency(48000*multiplier);}
	void Play(bool Looping);
	bool IsPlaying();
  bool operator==(const SoundComponent &sc);
private:
	SOUNDVECTOR Position, LastPos;
	SoundComponent(FMOD::System *system){sys = system;Volume = 1.0f;}
	FMOD::Channel *channel;
	FMOD::Sound *sound;
	FMOD::System *sys;

	void PlaySong(FMOD::Sound *_song, float volume = 1.0f );
	
  void PlaySound(FMOD::Sound *_sound, float volume = 1.0f);
  //unsigned int Position;
  const char *name;
	bool IsSong;
	float Volume;
};


class SoundEngine
{
public:
  SoundEngine();
  ~SoundEngine();

	SoundComponent *CreateSoundComp(const char *soundName);
	void DestroySoundComp(SoundComponent * sc);

  void Update(float timeDeltamS);

	void ChangeListenerAtts(const SOUNDVECTOR &_Position,
													const SOUNDVECTOR &_Forward,
													const SOUNDVECTOR &_Up);
  
  void PauseAll();
	void UnpauseAll();
	
  void MuteMusic(bool toMute);
	bool MusicMuted();

private:

	FMOD::System *mSystem;

	SOUNDVECTOR Position, LastPos, Forward, Up;
	
	unsigned int     version;
	int              numdrivers;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS        caps;
	char             name[256];

	

	void LoadSong(const std::string &filename, const char *songName);
	void LoadSound(const std::string &filename, const char *soundName);

  void FreeSound(const char *soundName);
  void FreeAllSounds();

	bool mMuteMusic;

	std::list<SoundComponent*> components;
  

	std::map<const char *, FMOD::Sound *> SoundHandles;





};