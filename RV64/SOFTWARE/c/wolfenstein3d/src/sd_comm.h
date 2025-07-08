#ifndef	__SD_COMM_H__
#define	__SD_COMM_H__

#define PACKED __attribute__((packed))

#define	TickBase	70	/* 70Hz per tick */

typedef	enum	{
					sdm_Off,
					sdm_PC,sdm_AdLib
				}	SDMode;
typedef	enum	{
					smm_Off,smm_AdLib
				}	SMMode;
typedef	enum	{
					sds_Off,sds_PC,sds_SoundBlaster
				}	SDSMode;
typedef	struct {
	longword length;
	word priority;
} PACKED SoundCommon;

typedef	struct {
	SoundCommon common;
	byte data[1];
} PACKED PCSound;

typedef	struct {
	byte mChar, cChar, mScale, cScale, mAttack, cAttack, mSus, cSus,
		mWave, cWave, nConn, voice, mode, unused[3];
} PACKED Instrument;

typedef	struct {
	SoundCommon common;
	Instrument inst;
	byte block, data[1];
} PACKED AdLibSound;

typedef	struct {
	word length, values[1];
} PACKED MusicGroup;

extern boolean AdLibPresent, SoundBlasterPresent;

extern	SDMode		SoundMode;
extern	SDSMode		DigiMode;
extern	SDMode		MusicMode;
extern	int		DigiMap[];

extern void SD_Startup();
extern void SD_Shutdown();

extern boolean SD_PlaySound(soundnames sound);
extern void SD_StopSound(),
				SD_WaitSoundDone(),
				SD_StartMusic(int music),
				SD_MusicOn(),
				SD_MusicOff(),
				SD_FadeOutMusic();

extern	boolean	SD_MusicPlaying(),
				SD_SetSoundMode(SDMode mode),
				SD_SetMusicMode(SMMode mode);
extern	word	SD_SoundPlaying();

extern void SD_SetDigiDevice(SDSMode);
extern void SD_Poll();

void PlaySoundLocGlobal(word s, int id, fixed gx, fixed gy);
void UpdateSoundLoc(fixed x, fixed y, int angle);

#endif
