#pragma once
#include <stdio.h>
#include "Sound.hpp"

class SoundPlayer
{
	public:
	SoundPlayer();
	~SoundPlayer();
	void Init();
	void PlaySound();


	private:
	unsigned int pcm, tmp, dir;
	int rate, channels, seconds;
	snd_pcm_t* pcm_handle = nullptr;
	snd_pcm_hw_params_t* params = nullptr;
	snd_pcm_uframes_t frames;
	char* buff;
	int buff_size;
};
