#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <functional>
#include <sys/types.h>
#include <sys/stat.h>
#include "Sound.hpp"


class SoundRegistrer
{
	public:
	~SoundRegistrer();
	SoundRegistrer();
	void Register(std::function<bool()> stopCondition);

	private:
	void RegistrerLoop();
	std::thread registerThread;
	bool shouldStop;
	int rc;
	int size;
	snd_pcm_t* handle;
	snd_pcm_hw_params_t* params;
	unsigned int val;
	int dir;
	snd_pcm_uframes_t frames;
	char* buffer;
};