#include <iostream>
#include <vector>
#include <sndfile.hh>
#include "wav_effects.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {

    if (argc < 6) {
        cerr << "Usage: " << argv[0] << " <input file> <output file> <mode> <delay> <amplitude_eco>\n";
        return 1;
    }

    SndfileHandle sfhIn{argv[argc - 5]};
    if (sfhIn.error()) {
        cerr << "Error: invalid input file\n";
        return 1;
    }

    if ((sfhIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: file is not in WAV format\n";
        return 1;
    }

    if ((sfhIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: file is not in PCM_16 format\n";
        return 1;
    }

    SndfileHandle sfhOut{argv[argc - 4], SFM_WRITE, sfhIn.format(),
                         sfhIn.channels(), sfhIn.samplerate()};
    if (sfhOut.error()) {
        cerr << "Error: invalid output file\n";
        return 1;
    }

    EffectType effectType;
    string strEffect = argv[argc - 3];

    if (strEffect.compare("SINGLE") == 0)
        effectType = SINGLE;
    else if (strEffect.compare("MULTIPLE") == 0)
        effectType = MULTIPLE;
    else if (strEffect.compare("AMPLITUDE_MOD") == 0)
        effectType = AMPLITUDE_MOD;
    else if (strEffect.compare("TIME_VARYING") == 0)
        effectType = TIME_VARYING;
    else {
        cerr << "Error: invalid  mode\n"
                "Available modes are:\n"
                "    SINGLE\n"
                "    MULTIPLE\n"
                "    AMPLITUDE_MOD\n"
                "    TIME_VARYING\n";
        return 1;
    }

    // Assigns the delay argument
    double delay;
    try {
        delay = stod(argv[argc - 2]);
    }
    catch (...){
        cerr << "Error: invalid value on the delay of the echo\n";
        return 1;
    }

    // Assigns the ampEco argument
    double ampEco;
    try {
        ampEco = stod(argv[argc - 1]);
    }
    catch (...){
        cerr << "Error: invalid value on the amplitude of the echo\n";
        return 1;
    }

    // Calculates the number of samples in the delay time
    int k = delay * sfhIn.samplerate();

    size_t nFrames;

    vector<short> inSamples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    vector<short> outSamples(FRAMES_BUFFER_SIZE * sfhIn.channels());

    WAVEffects wavEffects = WAVEffects(sfhIn, effectType, k, ampEco, FRAMES_BUFFER_SIZE * sfhIn.channels());

    while((nFrames = sfhIn.readf(inSamples.data(), FRAMES_BUFFER_SIZE))){
        wavEffects.applyEffect(inSamples);
        sfhOut.writef(wavEffects.outSamples.data(), nFrames);
    }
}

