#ifndef LANG_H    
#define LANG_H

#include "fcm.h"
#include <cmath>
#include <vector>
#include <cstring>

using namespace std;

/*
    Function that calculates number of bits required to compress a target file using a finite context model from a reference file.
    MultipleLangFlag is used to also store the set of words contained in target file that belong to reference file language.

    Note: This function is used by a second implementation of locatelang.py. Detailed explanation was written in report document.
*/
tuple<double, vector<tuple<int,int>>> get_number_of_bits_required_to_compress_v1(FCM fcm_model, string target_file_name, set<wchar_t> target_alphabet, bool multipleLangFlag) {

    long double total_num_bits {0};
    int different_chars_in_target {0};

    for (wchar_t ch: target_alphabet) {
        if (fcm_model.alphabet.find(ch) == fcm_model.alphabet.end()) {
            different_chars_in_target += 1;
        }
    }

    double threshold {(double) -log2(fcm_model.alpha / (fcm_model.alpha * fcm_model.alphabet.size()))};

    vector<tuple<int,int>> words;
    int char_position_in_text {0};

    auto it = fcm_model.model.begin();
    advance(it, rand() % fcm_model.model.size());
    wstring current_context = it->first;

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      vector<tuple<int,int>> emptyVector {};
      tuple<double, vector<tuple<int,int>>> emptyReturnValue = make_tuple(NULL, emptyVector);
      return emptyReturnValue;
    }

    bool word_creation {true};  // Flag to tell that a word is being created
    wstring new_word {L""};
    double word_total_bits {0};
    int initial_position {0};
    bool valid_word {true};
    
    wchar_t ch;
    while (file.get(ch)) {
        double num_bits {0};

        if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {

            if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
            } else {
                num_bits = -log2( (double) 1 / different_chars_in_target);
            }
        } else {
            num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()));
        }

        total_num_bits += num_bits;

        if (multipleLangFlag) {
            if (word_creation) {    // If a word is being created
                if (ch != ' ' && ch != '\n' ) {
                    if (num_bits<threshold) {   // If the char is compressed using a number of bits below the threshold
                        new_word += ch;         // Append the char to the word
                        word_total_bits += num_bits;    // Add the number of bits of this char to the word's total number of bits
                    } else {
                        valid_word = false;     // Flag to tell that this word has symbols that are not in this language, that's why it will not be saved
                    }
                } else {
                    if (valid_word && new_word.length() > 2) {  // Check if the word is valid. 1 - If it has no foreign symbols; 2 - If it has more than two chars
                        words.push_back( make_tuple(initial_position + 1, char_position_in_text) );
                    }
                    word_creation = false;  // Word creation process ended
                }
            }

            if (ch == ' ' && !(word_creation)) {    // This means that a new word will be created
                new_word = L"";                     // String for creating the word
                word_total_bits = 0;                // Total number of bits of the word
                initial_position = char_position_in_text;   // Initial position of the word in text
                valid_word = true;                  // Flag used to tell that the word has symbols that are not in this language
                word_creation = true;               // Flag to tell that a new word will be created
            }
        }

        // Update the context
        current_context.erase(0,1);
        current_context += ch;
        char_position_in_text += 1;
    }

    file.close();

    tuple <double, vector<tuple<int,int>>> returnValue(total_num_bits, words);

    return returnValue;

}



/*
    Function that calculates number of bits required to compress a target file using a finite context model from a reference file.
    MultipleLangFlag is used to also store the sections of chars contained in target file that are well compressed.
*/
tuple<double, vector<tuple<int,int>>> get_number_of_bits_required_to_compress_v2(FCM fcm_model, string target_file_name, set<wchar_t> target_alphabet, int window_size, bool multipleLangFlag) {

    long double total_num_bits {0};
    int different_chars_in_target {0};

    // Calculate the number of different chars between the two alphabets
    for (wchar_t ch: target_alphabet) {
        if (fcm_model.alphabet.find(ch) == fcm_model.alphabet.end()) {
            different_chars_in_target += 1;
        }
    }

    // Threshold value to determine if a section is well compressed or not
    double threshold {(double) log2( target_alphabet.size() ) / 2 };

    vector<tuple<int,int>> sections_dict;

    // Choose a random first context
    auto it = fcm_model.model.begin();
    advance(it, rand() % fcm_model.model.size());
    wstring current_context = it->first;

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      vector<tuple<int,int>> emptyVector {};
      tuple<double, vector<tuple<int,int>>> emptyReturnValue = make_tuple(NULL, emptyVector);
      return emptyReturnValue;
    }

    int window_initial_pos {0};
    vector<double> window_buffer = {};  // Window buffer
    for (int i = 0; i<window_size; i++ ) {
        window_buffer.push_back(0.0);
    }
    
    wchar_t * initial_chars = new wchar_t [window_size];
    file.read(initial_chars, window_size);

    wchar_t ch;
    for (int i=0; i<window_size; i++ ){
        ch = initial_chars[i];
        double num_bits;
        // If the current context exists in fcm model
        if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {
            // If char exists in the fcm model
            if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
            // If char doesn't exist in fcm alphabet
            } else {
                num_bits = -log2( (double) 1 / different_chars_in_target);
            }
        // If the current context does not exist in fcm model
        } else {
            num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()) );
        }

        total_num_bits += num_bits;
        window_buffer[i] = num_bits;       // Save position and num_bits in buffer
        // Update the context
        current_context.erase(0,1);
        current_context += ch;
    }

    double window_total_bits {0};
    if (multipleLangFlag) {
        // Sum of number of bits of all chars in current sliding window
        for (int i = 0; i<window_size; i++) {
            window_total_bits += window_buffer[i];
        }
        // If the average value of number of bits of this section is less than the threshold
        if ( ((double) window_total_bits/window_size) < threshold) {
            sections_dict.push_back( make_tuple(window_initial_pos, window_initial_pos+window_size) );
        }
    }

    // For the next chars, do the same process
    while (file.get(ch)) {
        window_initial_pos += 1;
        double num_bits {0};
        
        if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {
            if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
            } else {
                num_bits = -log2( (double) 1 / different_chars_in_target);
            }
        } else {
            num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()) );
        }

        total_num_bits += num_bits;
        
        if (multipleLangFlag) {
            // Get the char position in buffer that does not belong to this new section
            int previous_char_index = (window_initial_pos + window_size - 1) % window_size;
            // Remove the number of bits of this char to the window_total_bits
            window_total_bits -= window_buffer[previous_char_index];
            // Sum the number of bits of the new char to the window_total_bits
            window_total_bits += num_bits;
            // Save the number of bits of the new char in the previous char position
            window_buffer[previous_char_index] = num_bits;

            // If the average value of number of bits of this section is less than the threshold
            if (((double) window_total_bits/window_size) < threshold) {
                sections_dict.push_back( make_tuple(window_initial_pos, window_initial_pos+window_size) );
            }

        }
        
        // Update the context
        current_context.erase(0,1);
        current_context += ch;
    }

    file.close();

    tuple <double, vector<tuple<int,int>>> returnValue(total_num_bits, sections_dict);

    return returnValue;
}



/*
    Function that calculates number of bits required to compress a target file using multiple finite context models from a reference file.
*/
double get_number_of_bits_required_to_compress_multiplemodel(FCM first_fcm_model, FCM second_fcm_model, string target_file_name, set<wchar_t> target_alphabet) {
    double total_num_bits {0};
    double percentage_fist_model {0.5};
    double percentage_second_model {0.5};
    int different_chars_in_target {0};

    // Calculate the number of different chars between the two alphabets (Since both models are created from same file, the value will be the same in both models.)
    for (wchar_t ch: target_alphabet) {
        if (first_fcm_model.alphabet.find(ch) == first_fcm_model.alphabet.end()) {
            different_chars_in_target += 1;
        }
    }

    // Choose a random first context from second_fcm_model since it requires a bigger context (k=4)
    auto it_first_model = first_fcm_model.model.begin();
    advance(it_first_model, rand() % first_fcm_model.model.size());
    wstring current_context_first_model = it_first_model->first;

    auto it_second_model = second_fcm_model.model.begin();
    advance(it_second_model, rand() % second_fcm_model.model.size());
    wstring current_context_second_model = it_second_model->first;


    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      return -1.0;
    }

    wchar_t ch;
    while (file.get(ch)) {
        
        double num_bits_first_model {0};
        double num_bits_second_model {0};

        // Calculate required bits with first model
        if (first_fcm_model.state_probabilities.find(current_context_first_model) != first_fcm_model.state_probabilities.end()) {
        
            if (first_fcm_model.state_probabilities[current_context_first_model].find(ch) != first_fcm_model.state_probabilities[current_context_first_model].end()) {
                num_bits_first_model = -log2( first_fcm_model.state_probabilities[current_context_first_model][ch] );
            } else {
                
                num_bits_first_model = -log2( (double) 1 / different_chars_in_target);
            }
        } else {
            num_bits_first_model = -log2( (double) first_fcm_model.alpha / (first_fcm_model.alpha*first_fcm_model.alphabet.size()) );
        }

        // Calculate required bits with second model
        if (second_fcm_model.state_probabilities.find(current_context_second_model) != second_fcm_model.state_probabilities.end()) {
        
            if (second_fcm_model.state_probabilities[current_context_second_model].find(ch) != second_fcm_model.state_probabilities[current_context_second_model].end()) {
                num_bits_second_model = -log2( second_fcm_model.state_probabilities[current_context_second_model][ch] );
            } else {
                num_bits_second_model = -log2( (double) 1 / different_chars_in_target);
            }
        } else {
            num_bits_second_model = -log2( (double) second_fcm_model.alpha / (second_fcm_model.alpha*second_fcm_model.alphabet.size()) );
        }

        total_num_bits += (percentage_fist_model*num_bits_first_model) + (percentage_second_model*num_bits_second_model);

        // Update percentage of each model
        if ((num_bits_first_model < num_bits_second_model) && (percentage_fist_model <= 0.97)) {
            percentage_fist_model += 0.02;
            percentage_second_model -= 0.02;
        } else if ( (num_bits_second_model < num_bits_first_model) && (percentage_second_model <= 0.97) ) {
            percentage_fist_model -= 0.02;
            percentage_second_model += 0.02;
        }

        current_context_first_model.erase(0,1);
        current_context_first_model += ch;

        current_context_second_model.erase(0,1);
        current_context_second_model += ch;
    }

    file.close();

    return total_num_bits;
}

/*
    Function that returns the sections that are well compressed using a higher threshold.
*/
tuple<double, vector<tuple<int,int>>> get_sections_from_remaining_sections(FCM fcm_model, string target_file_name, set<wchar_t> target_alphabet, int window_size, vector<tuple<int,int>> nonlangsections) {

    double total_num_bits = 0;
    int different_chars_in_target = 0;

    // Calculate the number of different chars between the two alphabets
    for (wchar_t ch: target_alphabet) {
        if (target_alphabet.find(ch) == target_alphabet.end()) {
            different_chars_in_target += 1;
        }
    }

    vector<tuple<int,int>> sections_dict;

    // Higher Threshold value to determine if a section is well compressed or not
    double threshold = (3*log2(target_alphabet.size())) / 4;

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      vector<tuple<int,int>> emptyVector {};
      tuple<double, vector<tuple<int,int>>> emptyReturnValue = make_tuple(NULL, emptyVector);
      return emptyReturnValue;
    }

    int initial_char_in_line_position {0};
    wstring line;
    getline(file, line);    // Read initial line
    int number_of_chars_read = line.size();
    for (tuple<int,int> section: nonlangsections) {
        int section_initial_pos = get<0>(section);      // Get section initial position
        int section_final_pos =  get<1>(section);       // Get section final position
        int window_initial_pos = section_initial_pos;   // Sliding window inital position
        int window_final_pos = section_initial_pos;     // Current char position in text
        wstring section_chars {L""};                    // String to store the section chars
        wstring current_context;

        // Get section_chars and context
        while (true) {

            // Convert section position in target text to line index position
            int initial_section_position_in_line_string = section_initial_pos - initial_char_in_line_position;
            int final_section_position_in_line_string = section_final_pos - initial_char_in_line_position;
            // If section is in the current line
            if (section_final_pos < number_of_chars_read) {
                if (section_initial_pos - window_size < 0) {
                    // Choose a random first context
                    auto it = fcm_model.model.begin();
                    advance(it, rand() % fcm_model.model.size());
                    current_context = it->first;
                    // Get all the chars of the section
                    section_chars = line.substr(initial_section_position_in_line_string, final_section_position_in_line_string);
                } else {
                    // Read the original context
                    current_context = line.substr(initial_section_position_in_line_string - window_size, initial_section_position_in_line_string);
                    // Get all the chars of the section
                    section_chars = line.substr(initial_section_position_in_line_string, final_section_position_in_line_string);
                }
                break;
            // If section is between two lines
            } else if (section_final_pos > number_of_chars_read && section_initial_pos < number_of_chars_read) {
                // Read the original context
                current_context = line.substr(initial_section_position_in_line_string - window_size, initial_section_position_in_line_string);
                // Read the chars from the current line
                section_chars = line.substr(initial_section_position_in_line_string);

                // Read next line
                getline(file, line);
                initial_char_in_line_position = number_of_chars_read;
                number_of_chars_read += line.size();

                // Read the chars from the next line
                section_chars += line.substr(0, section_final_pos - initial_char_in_line_position);
                break;
            // If section is not in the current line we read another line
            } else {
                getline(file, line);
                initial_char_in_line_position = number_of_chars_read;
                number_of_chars_read += line.size();
            }
        }

        // Initialize buffer
        vector<double> window_buffer = {};
        for (int i = 0; i<window_size; i++ ) {
            window_buffer.push_back(0);
        }
        window_final_pos += window_size;        //  Increment the window_final_pos by window_size because we read the initial chars in the previous line

        for (int i = 0; i<window_size; i++) {
            wchar_t ch = section_chars[i];

            double num_bits {0};
            // If the current context exists in fcm model
            if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {
                // If the char exists in fcm model
                if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                    num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
                // If char doesn't exist in fcm alphabet
                } else {
                    num_bits = -log2( (double) 1 / different_chars_in_target);
                }
            // If the current context does not exist in fcm model
            } else {
                num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()) );
            }

            total_num_bits += num_bits;
            
            // Save position and num_bits in buffer
            window_buffer[i] = num_bits;
            // Update the context
            current_context.erase(0,1);
            current_context += ch;
        }

        // Sum of number of bits of all chars in current sliding window
        double window_total_bits {0};
        for (int i = 0; i<window_size; i++) {
            window_total_bits += window_buffer[i];
        }
        
        // If the average value of the current sliding value is less than the threshold
        if (((double)window_total_bits/window_size) < threshold) {
            sections_dict.push_back( make_tuple(window_initial_pos, window_final_pos) );
        }

        window_final_pos += 1;
        window_initial_pos += 1;

        // Analyse next chars of the section
        for (auto ch: section_chars.substr(window_size)) {

            double num_bits {0};
            if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {
        
                if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                    num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
                } else {
                    num_bits = -log2( (double) 1 / different_chars_in_target);
                }
            } else {
                num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()) );
            }

            total_num_bits += num_bits;

            // Get the char position in buffer that does not belong to this new section
            int previous_char_index = (window_initial_pos + window_size - 1) % window_size;
            
            // Remove the number of bits of this char to the window_total_bits
            window_total_bits -= window_buffer[previous_char_index];
            
            // Sum the number of bits of the new char to the window_total_bits
            window_total_bits += num_bits;

            // Save the number of bits of the new char in the previous char position
            window_buffer[previous_char_index] = num_bits;

            // If the average value of number of bits of this section is less than the threshold
            if (((double)window_total_bits/window_size) < threshold) {
                sections_dict.push_back( make_tuple(window_initial_pos, window_final_pos) );
            }

            // Update the context
            current_context.erase(0,1);
            current_context += ch;
            window_final_pos += 1;
            window_initial_pos += 1;
        }
    }

    file.close();
    tuple <double, vector<tuple<int,int>>> returnValue(total_num_bits, sections_dict);

    return returnValue;

}

tuple<double, vector<tuple<int,int>>> langCalculation(string input_file_name, string target_file_name, int k, double alpha, bool multipleLangFlag, bool multipleModelsFlag) {

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      vector<tuple<int,int>> emptyVector {};
      tuple<double, vector<tuple<int,int>>> emptyReturnValue = make_tuple(NULL, emptyVector);
      return emptyReturnValue;
    }

    wchar_t ch;             
    set<wchar_t> target_alphabet;

    // Get target alphabet
    while (file.get(ch)) {
        target_alphabet.insert(ch);
    }
    file.close();

    if (multipleModelsFlag) {
        FCM first_fcm_model {input_file_name, 2, alpha};
        first_fcm_model.calculate_probabilities();

        FCM second_fcm_model {input_file_name, 4, alpha};
        second_fcm_model.calculate_probabilities();

        double bits;
        bits = get_number_of_bits_required_to_compress_multiplemodel(first_fcm_model, second_fcm_model, target_file_name, target_alphabet);
        
        tuple <double, vector<tuple<int,int>>> returnValue(bits, {});

        return returnValue;
    } else {
        // Creation of Finite Context Model from reference text
        FCM fcm_model {input_file_name, k, alpha};

        // Calculate entropy
        fcm_model.calculate_probabilities();

        double bits;
        vector<tuple<int,int>> sections;
        tuple<double, vector<tuple<int,int>>> return_val = get_number_of_bits_required_to_compress_v2(fcm_model, target_file_name, target_alphabet, k, multipleLangFlag);

        // Following line needs to be uncommented in order to use second implementation of locatelang.py. Detailed explanation was written in report document.
        //tuple<double, vector<tuple<int,int>>> return_val = get_number_of_bits_required_to_compress_v1(fcm_model, target_file_name, target_alphabet, multipleLangFlag);
        
        bits = get<0>(return_val);
        sections = get<1>(return_val);

        tuple <double, vector<tuple<int,int>>> returnValue(bits, sections);
        return returnValue;
    }
}

#endif
