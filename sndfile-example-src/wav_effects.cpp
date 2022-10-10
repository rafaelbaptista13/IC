#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>
#include <queue>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {


    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <input file> <output file> <delay>\n";
        return 1;
    }

    SndfileHandle sfhIn{argv[argc - 3]};
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

    SndfileHandle sfhOut{argv[argc - 2], SFM_WRITE, sfhIn.format(),
                         sfhIn.channels(), sfhIn.samplerate()};
    if (sfhOut.error()) {
        cerr << "Error: invalid output file\n";
        return 1;
    }

    double delay = stod(argv[argc - 1]);

    int sampleNumber = delay * sfhIn.samplerate();

    size_t nFrames;

    int queue_size = 0;
    queue<int> delayBuffer;
    vector<short> inSamples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    vector<short> outSamples(FRAMES_BUFFER_SIZE * sfhIn.channels());

    while((nFrames = sfhIn.readf(inSamples.data(), FRAMES_BUFFER_SIZE))){
        for (auto it = inSamples.begin(); it != inSamples.end(); ++it) {
            int index = distance(inSamples.begin(), it);
            if (queue_size < sampleNumber * sfhIn.channels() - 1){
                delayBuffer.push(inSamples[index]);
                outSamples[index] = inSamples[index];
                queue_size++;
            }
            else{
                outSamples[index] = inSamples[index] + 0.1 * (double) delayBuffer.front();
                //cout << index << '\t' << inSamples[index]  << '\t'<< delayBuffer.front() << '\t' << outSamples[index] << '\n';
                delayBuffer.pop();
                delayBuffer.push(inSamples[index]);
            }

        }
        sfhOut.writef(outSamples.data(), nFrames);
    }
}