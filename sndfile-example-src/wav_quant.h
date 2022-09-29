#ifndef WAVQUANT_H
#define WAVQUANT_H

#include <iostream>
#include <map>
#include <math.h>

class WAVQuant {
  private:
	std::map<short, short> approximationMap;

  public:
	WAVQuant(const int num_bits) {
        int interval = pow(2, (16-num_bits));

        short currentThreshold = -32678;
        for (short i = -32678; i<= 32677; i++) {
            if (i % interval == 0) 
                currentThreshold = i;
            approximationMap[i] = currentThreshold;
        }

	}

	std::vector<short>& quantization(std::vector<short>& samples) {

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);

            samples[index] = approximationMap[samples[index]];
        }
        /*
        for(auto i = 0; i<samples.size(); i++) {
            samples[i] = approximationMap[samples[i]];
        }
        */
        return samples;
	}
};

#endif

