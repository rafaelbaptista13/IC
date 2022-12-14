#ifndef WAVQUANT_H
#define WAVQUANT_H

#include <iostream>
#include <map>
#include <math.h>

class WAVQuant {
  private:
	std::map<short, int> approximationMap;

  public:
	WAVQuant(const int num_bits, const int version) {
        int interval = pow(2, (16-num_bits));

        if (version == 2) {
            /*  Version of keeping the 0  (-8 -4 0 4) */
            int counter = 0;  
            int currentThreshold = -32768;
            for (int i = -32768; i<= 32767; i++) {
                if (i % interval == 0) { 
                    currentThreshold = i;    
                    counter = 0;
                }
                // If the current value is between two multiples of interval value, change the currentThreshold to the next multiple
                if (counter == (interval/2) + 1) {
                    // If to make sure that the new threshold is inside the range of amplitudes
                    // (If the new threshold is greater than the max amplitude, the threshold needs to remain the same)
                    if (currentThreshold + interval < 32767) 
                        currentThreshold = currentThreshold + interval;
                }
                approximationMap[i] = currentThreshold;
                counter++;
                //std::cout << i << "\t" << currentThreshold << '\n';
            }
        }
        
        if (version == 1) {
            /* Version of not keeping the 0   (-6 -2 2 6) */
            int counter = 0;  
            int currentThreshold = -32768 + interval / 2;
            for (int i = -32768; i<= 32767; i++) {
                // If the current value is in the middle of two multiples(of the interval value)
                if (abs(i % interval) == (interval / 2)) { 
                    currentThreshold = i;    
                    counter = 0;
                }
                if (counter == (interval/2)) {
                    currentThreshold = currentThreshold + interval;
                }
                approximationMap[i] = currentThreshold;
                counter++;
                //std::cout << i << "\t" << currentThreshold << '\n';
            }
        }
        
	}

	std::vector<short>& quantization(std::vector<short>& samples) {

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);

            samples[index] = approximationMap[samples[index]];
        }
        return samples;
	}
};

#endif

