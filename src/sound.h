////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//										MUSIC and SOUND FX
//
//				Written by Christian Eckhardt, ceckhard@calpoly.edu
//
//
//				Usage:
//
//				1. Get the sound.cpp and sound.h into your project and include the sound.h into your code: 
//
///					#include "sound.h"
//
//				2.	Make sure you have the "strmiids.lib" in your system
//
///					!! most likely, you have it already through the WindowsSDK !!
///					"strmiids.lib" is part DirectX SDK, if you installed DirectX SDK via platform SDK, 
///					then double check you have installed them correctly : http://msdn.microsoft.com/vstudio/express/visualc/usingpsdk/, especially step #3.
///					You can still use it with your OpenGL project though!
//
//				3.	Make a global object from the class music_
//
///						music_ music;
//
//				4. Start laser beams or explosion FX sounds (not music) anytime with
//
///						music.play_fx("laser.mp3");
//
//				5.	Initialize all music titles (mp3 files) you have. Sound FX (short files) do not have to be initialized, only music!				
//					A good place would be in the InitGeom() or any init before (!) the program loop, because do this only once:
//
///						int track1 = music.init_music("ariane grande.mp3");
///						int track2 = music.init_music("taylor swift.mp3");
///						int track3 = music.init_music("your pop trash.mp3");
//
///					Save the track number in an int ! You need it later!
//				
//				6. Start the music anytime with 
//
///						music.play(track2);						//or
///						music.fade_in_and_play(track3,3000);	//fade in for 3000 milliseconds = 3 seconds
//						
//				7. You have controll over fade in/out if necessary with
//
///						music.fade_in(track2);
///						music.fade_out(track2);
//
//				8. Activating autofade for fading out all other music when starting a new one with
//
///						music.set_auto_fadein_fadeout(true);
//
//
////////////////////////  (c) christian eckhardt  //////////////////////////////////////////////////



#pragma once

#include <dshow.h>
#include <cstdio>
#include <vector>
#include "minmax.h"
#pragma comment(lib, "strmiids.lib") 
#define VOLUME_FX -1200

wchar_t *GetWC(char *c);
class audio_args_
	{
	private:
		long fade_out;
	public:
		LPWSTR file;
		int volume;
		IBasicAudio *pBasicAudio;
		
		long start_fade;
		audio_args_()
			{
			fade_out = 0;
			file = NULL;
			volume = 0;
			pBasicAudio = NULL;
			start_fade = -1;
			}
		void fadeout(long timeset)
			{
			if (fade_out > 0)return;
			fade_out = timeset;
			start_fade = 0;
			}
		void fadein(long timeset)
			{
			if (fade_out < 0)return;
			fade_out = -timeset;
			start_fade = 0;
			}
		void process(long time_since_start)
			{
			if (fade_out > 0)
				{
				if (start_fade == 0)start_fade = time_since_start-1;
				long diff = time_since_start - start_fade;
				float vol = (float)(fade_out - diff) / (float)fade_out;
				vol = 1.0 - vol;
				vol = max((float)0, (float)vol);
				vol = min((float)1, (float)vol);
				vol *= -10000;
				vol = min((float)vol, (float)volume);
				set_volume(vol);
				}
			else if (fade_out < 0)//fadein
				{
				if (start_fade == 0)start_fade = time_since_start-1;
				long diff = time_since_start - start_fade;
				float vol = (float)(abs(fade_out) - diff) / (float)abs(fade_out);
				//vol = vol;
				vol = max((float)0, (float)vol);
				vol = min((float)1, (float)vol);

				vol *= -10000;
				vol = max((float)vol, (float)volume);
				set_volume(vol);
				}
			}
		void set_volume(int v)
			{
			volume = v;
			if (pBasicAudio)	pBasicAudio->put_Volume(v);
			}
		bool is_running()
			{
			if (pBasicAudio) return TRUE;
			return FALSE;
			}
	};

void start_music(LPWSTR file,int volume=0); 
void start_music(audio_args_ *audio_args);

class track_
	{
	private:
		audio_args_ *audio_args;
		wchar_t file[500];
	public:
		int num;
		//------------------------------------------------------------------------------------------
		track_()
			{
			num = -1;
			audio_args = NULL;
			}
		//------------------------------------------------------------------------------------------
		void operator=(const track_ rhs)
			{
			num = rhs.num;
			audio_args = rhs.audio_args;
			wcscpy(file, rhs.file);
			}
		bool is_running() { if (audio_args)return audio_args->is_running(); return FALSE; }
		//------------------------------------------------------------------------------------------
		void run()
			{
			if (audio_args)
				{
				if (audio_args->is_running()) return;
				delete audio_args;
				}
			audio_args = new audio_args_;
			audio_args->file = file;
			start_music(audio_args);
			}
		//------------------------------------------------------------------------------------------
		void set_file(wchar_t *datei)
			{
			wcscpy(file, datei);
			}
		//------------------------------------------------------------------------------------------
		void fade_out(int t)
			{
			if (audio_args) audio_args->fadeout(t);
			}
		//------------------------------------------------------------------------------------------
		void fade_in(int t)
			{
			if (audio_args) audio_args->fadein(t);
			}
		//------------------------------------------------------------------------------------------
		void process(long t)
			{
			if (audio_args) audio_args->process(t);
			}


	};
//------------------------------------------------------------------------------------------
class music_
	{
	private:
		std::vector<track_> tracks;
		int num;
		track_ *get_track(int nr)
			{
			for (int ii = 0; ii < tracks.size(); ii++)
				if (tracks[ii].num == nr)
					return &tracks[ii];
			return NULL;
			}
		bool autofade;
		void autofade_process(track_ *actual)
			{
			if (!autofade) return;
			for (int ii = 0; ii < tracks.size(); ii++)
				if (&tracks[ii] != actual)
					{
					tracks[ii].fade_out(3000);
					}
			}
	public:
		void set_auto_fadein_fadeout(bool set)
			{
			autofade = set;
			}
		int init_music(char *file)
			{
			track_ track;
			num++;
			track.num = num;
			track.set_file(GetWC(file));
			tracks.push_back(track);
			return num;
			}
		bool play(int title_no)
			{
			track_ *t = get_track(title_no);
			if (!t)return false;
			t->run();
			return true;
			}
		bool fade_in_and_play(int title_no,int fade_in_milli_seconds)
			{
			track_ *t = get_track(title_no);
			if (!t)return false;
			t->fade_in(fade_in_milli_seconds);
			t->run();
			return true;
			}
		bool fade_in(int title_no, int fade_in_milli_seconds)
			{
			track_ *t = get_track(title_no);
			if (!t)return false;
			t->fade_in(fade_in_milli_seconds);
			return true;
			}
		bool fade_out(int title_no, int fade_out_milli_seconds)
			{
			track_ *t = get_track(title_no);
			if (!t)return false;
			t->fade_out(fade_out_milli_seconds);
			return true;
			}
		void play_fx(char *file)
			{
			start_music(GetWC(file), VOLUME_FX);
			}
		music_()
			{
			autofade = false;
			num = -1;
			}
	};
const wchar_t *GetWC(const char *c);
