#include "Core/AudioPlayer.h"
#include "Core/Assert.h"
#include "Audio/AudioSubsystem.h"
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>

AudioPlayer::AudioPlayer()
{
    ma_engine_play_sound((ma_engine*)AudioSubsystem::GetInternalHandler(), "C:/Users/User/Desktop/miniaudio-master/examples/sound.wav", nullptr);
}

AudioPlayer::~AudioPlayer()
{
    
}
