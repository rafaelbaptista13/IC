#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>

class WAVHist {
  private:
	std::vector<std::map<short, size_t>> counts;
	std::map<int, size_t> midChannelCounts;
	std::map<int, size_t> sideChannelCounts;
	bool midChannelRequest;
	bool sideChannelRequest;

  public:
	WAVHist(const SndfileHandle& sfh, bool midChannelRequest, bool sideChannelRequest) {
		counts.resize(sfh.channels());
		this->midChannelRequest = midChannelRequest;
		this->sideChannelRequest = sideChannelRequest;
	}

	void update(const std::vector<short>& samples) {
		if (midChannelRequest) {
			/*
			 MID Channel Histogram Calculation
			*/
			int lastSample = 0;
			size_t n { };
			
			for(auto s : samples) {
				if (n++ % 2 == 0) {
					lastSample = s;
				} else {
					int meanValue = (lastSample + s) / 2;
					midChannelCounts[meanValue]++;
				}
			}
		}
		else if (sideChannelRequest) {
			/*
			 SIDE Channel Histogram Calculation
			*/
			int lastSample = 0;
			size_t n { };
			for(auto s : samples) {
				if (n++ % 2 == 0) {
					lastSample = s;
				} else {
					int diffValue = (lastSample - s) / 2;
					sideChannelCounts[diffValue]++;
				}
			}
		}
		else {
			/*
			 Specific Channel Histogram Calculation
			*/
			size_t n { };
			for(auto s : samples)
				counts[n++ % counts.size()][s]++;
		}

	}

	void dump(const size_t channel) const {

		if (midChannelRequest) {
			for(auto [value, counter] : midChannelCounts)
				std::cout << value << '\t' << counter << '\n';
		}
		else if (sideChannelRequest) {
			for(auto [value, counter] : sideChannelCounts)
				std::cout << value << '\t' << counter << '\n';
		}
		else {
			for(auto [value, counter] : counts[channel])
				std::cout << value << '\t' << counter << '\n';
		}
	}
};

#endif

