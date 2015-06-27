//------------------------------------------------------------------------
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------
#include "sound.h"
#include "fmod_errors.h"

#pragma message("Building Sound Engine")

#define ERRCHECK(x) if (x != FMOD_OK) \
	{ \
	printf("FMOD error! (%d) %s\n", x, FMOD_ErrorString(x)); \
	exit(-1); \
	}


SoundEngine::SoundEngine()
{
	FMOD_RESULT      result;
	result = FMOD::System_Create(&mSystem);
	ERRCHECK(result);

	result = mSystem->getVersion(&version);
	ERRCHECK(result);

	if (version < FMOD_VERSION)
	{
		printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
		exit(0);
	}

	result = mSystem->getNumDrivers(&numdrivers);
	ERRCHECK(result);

	if (numdrivers == 0)
	{
		result = mSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ERRCHECK(result);
	}
	else
	{
		result = mSystem->getDriverCaps(0, &caps, 0, 0, &speakermode);
		ERRCHECK(result);

		result = mSystem->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
		ERRCHECK(result);

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
		{                                                   /* You might want to warn the user about this. */
			result = mSystem->setDSPBufferSize(1024, 10);
			ERRCHECK(result);
		}

		result = mSystem->getDriverInfo(0, name, 256, 0);
		ERRCHECK(result);

		if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
		{
			result = mSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
			ERRCHECK(result);
		}
	}

	result = mSystem->init(100, FMOD_INIT_NORMAL, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
	{
		result = mSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		ERRCHECK(result);

		result = mSystem->init(100, FMOD_INIT_NORMAL, 0);/* ... and re-init. */
		ERRCHECK(result);
	}


	//SPECIAL EFFECTS stuff here
	mSystem->set3DSettings(1.f, 1.0f, 1.f);
	FMOD_REVERB_PROPERTIES frp = FMOD_PRESET_OFF;
	mSystem->setReverbAmbientProperties(&frp);
	/*FMOD::Reverb *fr;
	mSystem->createReverb(&fr);
	*/

	//Load Songs and Sounds HERE


	std::string filePrefix = "Assets/Sound/";
	//LoadSong("Assets/Sound/bgm.wav","Background");
	LoadSong(filePrefix + "mscPiano with bass.mp3","piano");
  LoadSong(filePrefix + "mscMystick 2 Theme.mp3","Theme");
	LoadSong(filePrefix + "mscEasyLevel.mp3","EasyLevel");

	LoadSong(filePrefix + "32 - Lament.mp3","LoseMusic");
	LoadSong(filePrefix + "08 - Eye of the DJ.mp3","WinMusic");
	LoadSong(filePrefix + "14 - Distinction.mp3","Level1Music");
	LoadSong(filePrefix + "46 - Sole Groove.mp3","Level2Music");
	LoadSong(filePrefix + "26 - Almighty Dollar.mp3","Level3Music");
	//LoadSound("Assets/Sound/death.wav","death");
	LoadSound(filePrefix + "sfxShockwaveShort.mp3","Parry");
	LoadSound(filePrefix + "sfxfirst jump.mp3","First Jump");
	LoadSound(filePrefix + "Male_Jump_Grunt05.wav","JumpGrunt");
	LoadSound(filePrefix + "Male_Land_Grunt04.wav","LandGrunt");
	LoadSound(filePrefix + "Male_Parry_Grunt04.wav","ParryGrunt");
	LoadSound(filePrefix + "44 - Man Screams #3.mp3","DeathYell");
	LoadSound(filePrefix + "Male07_Hurt09.wav","HurtGrunt1");
	LoadSound(filePrefix + "Male07_Hurt23.wav","HurtGrunt2");

	LoadSound(filePrefix + "Beep_SynthTone02.wav","MenuSelect");
	LoadSound(filePrefix + "Beep_TextScroll1.wav","MenuChange");
	this->mMuteMusic = false;
}

SoundEngine::~SoundEngine()
{
	std::list<SoundComponent *>::iterator iter = components.begin();
	while( iter != components.end() )
	{
		delete *iter;
		++iter;
	}
	FreeAllSounds();
	mSystem->release();
}



void SoundEngine::LoadSong(const std::string &filename, const char *songName)
{
	FMOD_RESULT result;
	FMOD::Sound *sound;
	result = mSystem->createStream(filename.c_str(), FMOD_DEFAULT, 0, &sound);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	ERRCHECK(result);

	SoundHandles[songName] = sound;
}
void SoundEngine::LoadSound(const std::string &filename, const char *soundName)
{
	FMOD_RESULT result;
	FMOD::Sound *sound;
	result = mSystem->createSound(filename.c_str(), FMOD_3D, 0, &sound);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	ERRCHECK(result);
	SoundHandles[soundName] = sound;
}


void SoundEngine::FreeSound(const char *soundName)
{
	SoundHandles[soundName]->release();
	SoundHandles.erase(soundName);
}



void SoundEngine::FreeAllSounds()
{
	std::map<const char*, FMOD::Sound*>::iterator iter = SoundHandles.begin();
	while( iter != SoundHandles.end() )
	{
		iter->second->release();
		++iter;
	}
	SoundHandles.clear();
}



void SoundComponent::PlaySong(FMOD::Sound *_song, float volume)
{
	/*std::map<const char*, FMOD::Sound *>::const_iterator iter = SoundHandles.find(MusicName);
	if( iter == SoundHandles.end() )
		return NULL;*/

	FMOD_RESULT res;
	FMOD::Sound *s = _song;
	res = sys->playSound(FMOD_CHANNEL_FREE, s, true, &channel);
	ERRCHECK(res);
	channel->setVolume(Volume);
	//channel->set3DAttributes(&Position, &Velocity);
	channel->setPaused(false);
}
void SoundEngine::ChangeListenerAtts(const SOUNDVECTOR &_Position, const SOUNDVECTOR &_Forward, const SOUNDVECTOR &_Up)
{
	Position = _Position;
	Forward = _Forward;
	Up = _Up;
}
bool SoundComponent::IsPlaying()
{
	bool res; 
	FMOD_RESULT r;
  r = channel->isPlaying(&res);
	ERRCHECK(r);

	return res;
}

void SoundComponent::PlaySound(FMOD::Sound *_sound, float volume)
{
	/*std::map<const char*, FMOD::Sound *>::const_iterator iter = SoundHandles.find(SoundName);
	if( iter == SoundHandles.end() )
		return NULL;*/

	FMOD_RESULT res;
	FMOD::Sound *s = _sound;
	res = sys->playSound(FMOD_CHANNEL_FREE, s, true, &channel);
	ERRCHECK(res);
	channel->setVolume(Volume);
	channel->set3DAttributes(&Position, &SOUNDVECTOR(0,0,0));
	channel->setPaused(false);
	//channel->set3DAttributes(&Position, &Velocity);
}

SoundComponent *SoundEngine::CreateSoundComp(const char *soundName)
{
	SoundComponent *sc = new SoundComponent(mSystem);

	sc->channel = NULL;
	sc->name = soundName;
	sc->sound = SoundHandles[soundName];
	FMOD_MODE fm;
	sc->sound->getMode(&fm);
	if(fm & FMOD_3D )
		sc->IsSong = false;
	else
		sc->IsSong = true;

	this->components.push_back(sc);
	return sc;
}
void SoundEngine::DestroySoundComp(SoundComponent * sc)
{
	if(sc == NULL)// || *sc == NULL)
		return;
	
	
	components.remove( sc);
	delete  sc;
	//*sc = NULL;
}

float SoundComponent::GetSoundDuration()
{
	unsigned int length;
	this->sound->getLength(&length, FMOD_TIMEUNIT_MS);

	return length / 1000.0f;
}

void SoundComponent::Pause()
{
	
	this->channel->setPaused(true);
}
void SoundComponent::Stop()
{
	this->channel->stop();
}
void SoundComponent::Unpause()
{
	this->channel->setPaused(false);
}
bool SoundComponent::Paused()
{
	bool res;
	channel->getPaused(&res);
	return res;
}

void SoundComponent::Play(bool Looping)
{
	if( IsSong )
	{
		this->PlaySong(sound, Volume);
	}
	else
	{
		this->PlaySound(sound, Volume);
		Update3DAtts(Position);
	}
	if( Looping )
		this->channel->setMode(FMOD_LOOP_NORMAL);
	else
		this->channel->setMode(FMOD_LOOP_OFF);
}

void SoundComponent::Update3DAtts(const SOUNDVECTOR &pos)
{
	Position = pos;
	//Velocity = vel;
	/*if( IsPlaying() )
	{
		channel->set3DAttributes(&Position, &Velocity);
	}*/
}

bool SoundComponent::operator==(const SoundComponent &sc)
{
	return strcmp(name, sc.name) == 0;
}

void SoundEngine::MuteMusic(bool toMute)
{
	mMuteMusic = toMute;
}
bool SoundEngine::MusicMuted()
{
	return mMuteMusic;
}

void SoundEngine::PauseAll()
{	
	std::list<SoundComponent*>::iterator iter = components.begin();
	while( iter != components.end() )
	{
		(*iter++)->Pause();
	}
	
}
void SoundEngine::UnpauseAll()
{
	std::list<SoundComponent*>::iterator iter = components.begin();
	while( iter != components.end() )
	{
		(*iter++)->Unpause();
	}
}

void SoundEngine::Update(float timeDeltamS)
{
	std::list<SoundComponent*>::iterator iter = components.begin(), iter2;
	while( iter != components.end() )
	{
		iter2 = iter;
		++iter;
		if( (*iter2)->owner == NULL )
			this->DestroySoundComp( *iter2 );
		else
		{
			SoundComponent *s = (*iter2);
			if( s->IsSong )
				s->channel->setMute(mMuteMusic);
			s->channel->set3DAttributes(&s->Position, &((s->Position - s->LastPos) * (1000 / timeDeltamS)));
		}
	}
	SOUNDVECTOR vel = (Position - LastPos) * (1000 / timeDeltamS);
	LastPos = Position;
	mSystem->set3DListenerAttributes(0, &Position, &vel, &Forward, &Up);
	mSystem->update();

}
