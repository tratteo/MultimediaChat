
#include <alsa/asoundlib.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <functional>
#include <sys/types.h>
#include <sys/stat.h>

#define ALSA_PCM_NEW_HW_PARAMS_API
#define PCM_DEVICE "default"
#define CHANNELS 2
#define SAMPLE_RATE 44100
#define BUFFER_FILE "recording.data"

class SoundRegistrer
{
	public:
	~SoundRegistrer();
	SoundRegistrer();
	void Register(std::function<bool()> stopCondition);
	inline char* GetBuffer() const { return buffer; }

	private:
	void RegistrerLoop();
	std::thread registerThread;
	bool shouldStop;
	long loops;
	int rc;
	int size;
	snd_pcm_t* handle;
	snd_pcm_hw_params_t* params;
	unsigned int val;
	int dir;
	snd_pcm_uframes_t frames;
	char* buffer;
};