#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <numeric>

using namespace std;

class FCM {
  private:
    int k;
    double alpha;
    map<wstring, map<wchar_t, double>> state_probabilities;
    int number_of_states = 0;
    map<wstring, map<wchar_t, double>> model;
    set<wchar_t> alphabet;

  public:
	FCM(string reference_file_name, int k, double alpha) {
    this->k = k;
    this->alpha = alpha;
    this->state_probabilities = {};
    this->alphabet = {};
    this->model = {};
    // generate fcm model
    create_fcm_model(reference_file_name);
	}

	void create_fcm_model(string reference_file_name) {

    locale::global(locale(""));
    wifstream file(reference_file_name);
    if (!file.is_open()){
      cerr << "Invalid input file: '" << reference_file_name << "'" << endl;
      return;
    }

    wchar_t ch;       // Char
    wstring ctx;      // Context

    // Read first context
    for (int i = 0; i < this->k; i++) {
      file.get(ch);
      ctx += ch;
    }

    file.get(ch);   // Read first char

    do {
      //wcout << ch;
      alphabet.insert(ch);        // Add char to alphabet
      this->number_of_states++;   // Increment number of states

      if (this->model.count(ctx)) {
        if (this->model.at(ctx).count(ch)) {
          this->model.at(ctx).at(ch) += 1;
        }
        else 
          this->model.at(ctx).insert({ch, 1});
      } else {
        this->model.insert({ctx, {{ch, 1}}});
      }

      ctx.erase(0,1);
      ctx += ch;
    } while (file.get(ch));
  }

  void calculate_probabilities() {

    int alphabet_size = this->alphabet.size();

    this->state_probabilities = this->model;
    // Iterate all contexts
    for (auto state_pair: this->model) {
      wstring state = state_pair.first;
      map<wchar_t, double> state_info = this->model.at(state);

      // Sum all the occurences
      int state_sum = std::accumulate(state_info.begin(), state_info.end(), 0,
          [](const int prev_sum, const std::pair<wchar_t, double> &entry) {
            return prev_sum + entry.second;
          });

      // Number of not shown chars
      int number_of_not_shown_chars = alphabet_size - state_sum;

      if (number_of_not_shown_chars != 0) {   // If there is at least one char that did not appear
        // Probability calculation with alpha parameter 
        double prob_chars = this->alpha / (state_sum + this->alpha * alphabet_size );
        
        for (auto ch : this->alphabet) {
          this->state_probabilities[state][ch] = prob_chars;     // Save the probability
        } 
      }

      for (auto ch_occur_pair: state_info) {
        wchar_t ch = ch_occur_pair.first;
        // Probability calculation with alpha parameter 
        double prob_char = (state_info[ch] + this->alpha) / (state_sum + this->alpha * alphabet_size );
        this->state_probabilities[state][ch] = prob_char;         // Save the probability
      }
    }

  }

};