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
    //SndfileHandle& sfh;
    EffectType effectType;
    int frameCount;
    int k;
    double ampEco;
    int channels;
    int sampleRate;
    std::deque<int> delayBuffer;

    void eco_effect(short sample, int index){
        int kChannel = k * channels;
        if (delayBuffer.size() < kChannel - 1){
            delayBuffer.push_back(sample);
            outSamples[index] = sample;
        }
        else{
            switch (effectType) {
                case SINGLE:
                case MULTIPLE:
                    outSamples[index] = (sample + ampEco * (double) delayBuffer.front()) / (1 + ampEco);
                    //std::cout << index << '\t' << ampEco << '\t' << sample  << '\t'<< delayBuffer.front() << '\t' << outSamples[index] << '\n';
                    break;
                case TIME_VARYING:
                    outSamples[index] = (sample + ampEco * (double) delayBuffer[cos(frameCount*100/sampleRate)]) / (1 + ampEco);
                    break;
                default:
                    return;
            }
            //cout << index << '\t' << inSamples[index]  << '\t'<< delayBuffer.front() << '\t' << outSamples[index] << '\n';
            delayBuffer.pop_front();

            //cout << effectType << '\t' << MULTIPLE <<'\n';
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
                double result = (3000.0 / sampleRate) * frameCount * 2.0 * M_PI;
                //FIXME: change this 1000 to variable asked in the UI and see formula
                outSamples[index] = samples[index] * cos((1000.0 / sampleRate) * frameCount * 2.0 * M_PI);
                //std::cout << frameCount << "\t" << M_PI << "\t" << result << "\t"  << cos(result) << "\n";
            }
            if (index % 2 == 1)
                frameCount++;
        }
    }
};

#endif //IC_WAV_EFFECTS_H
