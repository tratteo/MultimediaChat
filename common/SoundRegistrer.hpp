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
	bool shouldStop = false;
	int size;
	snd_pcm_t* handle = nullptr;
	snd_pcm_hw_params_t* params = nullptr;
	snd_pcm_uframes_t frames;
	char* buffer;
};