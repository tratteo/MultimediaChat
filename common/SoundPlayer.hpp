#include <alsa/asoundlib.h>
#include <stdio.h>

#define ALSA_PCM_NEW_HW_PARAMS_API
#define PCM_DEVICE "default"
#define CHANNELS 2
#define SAMPLE_RATE 44100

class SoundPlayer
{
	public:
	SoundPlayer();
	~SoundPlayer();
	void Init();
	void PlaySound(int fd);


	private:
	unsigned int pcm, tmp, dir;
	int rate, channels, seconds;
	snd_pcm_t* pcm_handle;
	snd_pcm_hw_params_t* params;
	snd_pcm_uframes_t frames;
	char* buff;
	int buff_size, loops;
};
