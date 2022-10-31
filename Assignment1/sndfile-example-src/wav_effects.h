#ifndef IC_WAV_EFFECTS_H
#define IC_WAV_EFFECTS_H
#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <deque>
#include <math.h>

enum EffectType{
    SINGLE,
    MULTIPLE,
    AMPLITUDE_MOD,
    TIME_VARYING
};


class WAVEffects {
private:
    EffectType effectType;
    int frameCount;
    int k;
    double ampEco;
    int channels;
    int sampleRate;
    std::deque<int> delayBuffer;

    void eco_effect(short sample, int index){
        int kChannel = k * channels;
        if ((int) delayBuffer.size() < kChannel - 1){
            delayBuffer.push_back(sample);
            outSamples[index] = sample;
        }
        else{
            switch (effectType) {
                case SINGLE:
                case MULTIPLE:
                    outSamples[index] = (sample + ampEco * (double) delayBuffer.front()) / (1 + ampEco);
                    break;
                case TIME_VARYING:
                    outSamples[index] = (sample + ampEco *
                            (double) delayBuffer[abs(cos(frameCount*2.0*M_PI/sampleRate)*200)]) / (1 + ampEco);
                    break;
                default:
                    return;
            }
            delayBuffer.pop_front();
            if (effectType != MULTIPLE)
                delayBuffer.push_back(sample);
            else
                delayBuffer.push_back(outSamples[index]);
        }
    }

public:
    std::vector<short> outSamples;
    WAVEffects(SndfileHandle& sfh, EffectType effectType, int k, double ampEco, int outSampleSize){
        this->sampleRate = sfh.samplerate();
        this->channels = sfh.channels();
        this->frameCount = 0;
        this->effectType = effectType;
        this->ampEco = ampEco;
        this->k = k;
        this->outSamples = std::vector<short>(outSampleSize);
    }

    void applyEffect(const std::vector<short>& samples) {
        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
            if (effectType != AMPLITUDE_MOD){
                eco_effect( samples[index], index);
            }
            else {
                outSamples[index] = samples[index] * cos((1.5 / sampleRate) * frameCount * 2.0 * M_PI);
            }
            if (index % 2 == 1)
                frameCount++;
        }
    }
};

#endif //IC_WAV_EFFECTS_H
