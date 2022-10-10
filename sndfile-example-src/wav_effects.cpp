#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>
#include <deque>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

enum EffectType{
    SINGLE,
    MULTIPLE,
    AMPLITUDE_MOD,
    TIME_VARYING
};

void eco_effect(vector<short>& inSamples, vector<short>& outSamples, deque<int>& delayBuffer,
                const int kChannel, const double ampEco, const int index, const EffectType effectType){

    if (delayBuffer.size() < kChannel - 1){
        delayBuffer.push_back(inSamples[index]);
        outSamples[index] = inSamples[index];
    }
    else{
        switch (effectType) {
            case SINGLE:
            case MULTIPLE:
                outSamples[index] = (inSamples[index] + ampEco * (double) delayBuffer.front()) / (1 + ampEco);
                break;
            case TIME_VARYING:
                outSamples[index] = (inSamples[index] + ampEco * (double) delayBuffer[cos(kChannel)]) / (1 + ampEco);
                break;
            default:
                return;
        }
        //cout << index << '\t' << inSamples[index]  << '\t'<< delayBuffer.front() << '\t' << outSamples[index] << '\n';
        delayBuffer.pop_front();

        //cout << effectType << '\t' << MULTIPLE <<'\n';
        if (effectType != MULTIPLE)
            delayBuffer.push_back(inSamples[index]);
        else
            delayBuffer.push_back(outSamples[index]);
    }
}

int main(int argc, char *argv[]) {

    if (argc < 6) {
        cerr << "Usage: " << argv[0] << " <input file> <output file> <delay> <amplitude_eco> <mode> <\n";
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
    string strEffect = argv[argc - 1];

    if (strEffect.compare("SINGLE") == 0)
        effectType = SINGLE;
    else if (strEffect.compare("MULTIPLE") == 0)
        effectType = MULTIPLE;
    else if (strEffect.compare("AMPLITUDE_MOD") == 0)
        effectType = AMPLITUDE_MOD;
    else if (strEffect.compare("TIME_VARYING") == 0)
        effectType = TIME_VARYING;
    else {
        cerr << "Error: invalid output file\n";
        return 1;
    }

    //TODO: checks if it has errors
    double delay = stod(argv[argc - 3]);
    double ampEco = stod(argv[argc - 2]);

    int k = delay * sfhIn.samplerate();

    size_t nFrames;

    deque<int> delayBuffer;
    vector<short> inSamples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    vector<short> outSamples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    while((nFrames = sfhIn.readf(inSamples.data(), FRAMES_BUFFER_SIZE))){
        for (auto it = inSamples.begin(); it != inSamples.end(); ++it) {
            int index = distance(inSamples.begin(), it);
            if (effectType != AMPLITUDE_MOD){
                eco_effect( inSamples, outSamples, delayBuffer,
                           k*sfhIn.channels(), ampEco, index, effectType);
            }
            else {
                outSamples[index] = inSamples[index] * cos(1/sfhIn.samplerate() * nFrames);
            }
        }
        sfhOut.writef(outSamples.data(), nFrames);
    }
}

