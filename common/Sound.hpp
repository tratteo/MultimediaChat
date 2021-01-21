#pragma once
#include <alsa/asoundlib.h>
#include "SoundPlayer.hpp"
#include "SoundRegistrer.hpp"
#define DGRAM_PACKET_SIZE 1024
#define DGRAM_SEND_DELAY 500
#define ALSA_PCM_NEW_HW_PARAMS_API
#define CHANNELS 2
#define SAMPLE_RATE 44100
#define BUFFER_FILE "recording.data"
#define RECEIVED_FILE "received.data"
#define PCM_DEVICE "default"