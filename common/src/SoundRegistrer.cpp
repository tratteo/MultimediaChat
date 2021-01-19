#include "../SoundRegistrer.hpp"

SoundRegistrer::SoundRegistrer()
{
    /* Open PCM device for recording (capture). */
    rc = snd_pcm_open(&handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) 
    {
        fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
        exit(1);
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params, CHANNELS);

    /* 44100 bits/second sampling rate (CD quality) */
    val = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

    /* Set period size to 32 frames. */
    frames = 32;
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) 
    {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
        exit(1);
    }

    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);
    size = frames * 4; /* 2 bytes/sample, 2 channels */
    buffer = new char[size]; 

    /* We want to loop for 5 seconds */
    snd_pcm_hw_params_get_period_time(params, &val, &dir);
    shouldStop = false;
}


SoundRegistrer::~SoundRegistrer()
{
    delete[] buffer;
	snd_pcm_close(handle);
}

void SoundRegistrer::Register(std::function<bool()> stopCondition)
{
    if (registerThread.joinable())
    {
        registerThread.join();
    }

    registerThread = std::thread(&SoundRegistrer::RegistrerLoop, this);

    while (!shouldStop)
    {
        if (stopCondition())
        {
            shouldStop = true;
            continue;
        }
    }
    registerThread.join();
}

void SoundRegistrer::RegistrerLoop()
{
    std::ofstream outStream(BUFFER_FILE, std::ofstream::out | std::ofstream::trunc);
    if (outStream.fail())
    {
        std::cerr << "Error opening file for recording, skipping" << std::endl;
        return;
    }

    while (!shouldStop)
    {
        rc = snd_pcm_readi(handle, buffer, frames);
        if (rc == -EPIPE)
        {
            fprintf(stderr, "overrun occurred\n");
            snd_pcm_prepare(handle);
        }
        else if (rc < 0)
        {
            fprintf(stderr, "error from read: %s\n", snd_strerror(rc));
        }
        else if (rc != (int)frames)
        {
            fprintf(stderr, "short read, read %d frames\n", rc);
        }
        outStream.write(buffer, size);
    }
    outStream.close();
}
