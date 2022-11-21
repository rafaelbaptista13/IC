#ifndef GOLOMB_CODEC_H
#define GOLOMB_CODEC_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <bitset>
#include <algorithm>
#include "BitStream.h"

class GolombCode {
  private:
	int m;          // Divsion Factor
    std::vector<std::string> truncated_binary_codes;

  public:
    /*
        Class constructor. Creates vector with truncated binary codes according to m parameter.
            Arguments: m - golomb code m parameter
    */
	GolombCode(int m) {
		this->m = m;    // Golomb codes m parameter.

        int min_num_symbols_per_code = floor(log2(m));                  // Minimum length that truncated binary code will have.
        int max_num_symbols_per_code = min_num_symbols_per_code + 1;    // Maximum length that truncated binary code will have.

        int num_codes_with_min_num_symbols_per_code =  pow(2, min_num_symbols_per_code) - (m - pow(2, min_num_symbols_per_code));   // Number of codes that will have the minimum length in truncated binary code.
        int num_codes_with_max_num_symbols_per_code = m - num_codes_with_min_num_symbols_per_code;                                  // Number of codes that will have the maximum length in truncated binary code.

        std::string symbolcode; 
        
        // Create codes for the symbols that will have the minimum length.
        for (int i = 0; i < num_codes_with_min_num_symbols_per_code; i++ ) {
            // Encode number to 32-bit fixed binary code.
            symbolcode = std::bitset<32>(i).to_string();

            // Get the significant bits of the 32-bit code.
            symbolcode = symbolcode.substr(symbolcode.length() - min_num_symbols_per_code, symbolcode.length());

            // Store the code
            truncated_binary_codes.push_back(symbolcode);
        }

        if (num_codes_with_max_num_symbols_per_code > 0) {
            // Add a 0 to the end of the last binary code
            int last_num = num_codes_with_min_num_symbols_per_code << 1;    

            // Convert to binary string
            std::string code_first_symbol_with_max = std::bitset<32>( last_num ).to_string();
            code_first_symbol_with_max = code_first_symbol_with_max.substr(code_first_symbol_with_max.length() - max_num_symbols_per_code, code_first_symbol_with_max.length());
            
            // Store the code
            truncated_binary_codes.push_back(code_first_symbol_with_max);
            
            // Create the remaining codes with maximum lenght.
            for (int i = num_codes_with_min_num_symbols_per_code+1; i<m; i++) {
                int current_num = last_num + 1;
                symbolcode = std::bitset<32>(current_num).to_string();
                symbolcode = symbolcode.substr(symbolcode.length() - max_num_symbols_per_code, symbolcode.length());
                truncated_binary_codes.push_back(symbolcode);

                last_num = current_num;
            }
        
        }


	}

    /*
        Function to encode an integer number into a golomb code.
    */
	std::string encode(int number) {
		
        std::string code;

        int q, r;
        q = abs(number / m);    // quotient
        r = abs(number % m);    // rest

        // Create code. Unary code for quotient and truncated binary code for rest.
        code = encodeQuotient(q) + truncated_binary_codes[r];

        if (number > 0) code += "0";            // Add "0" bit for positive numbers
        else if (number < 0) code += "1";       // Add "1" bit for negative numbers.

        return code;
    }

    /*
        Function that encodes the quotient part of a golomb code.
    */
    std::string encodeQuotient(int quotient) {

        std::string code { "" };

        // Create string of length equal to the number to codify of "0"s
        for (int i = 0; i < quotient; i++) {
            code+="0";
        }

        // Add a "1" bit to the end of the string.
        code += "1";

        return code;
    }

    /*
        Function to decode a code with the full string of bits that made the code.
    */
    int decode(std::string number_code) {

        // Decode quotient part
		int q = 0;
        while (number_code[q] != '1') {
            q++;
        }

        // Decode truncated binary code
        int size_of_rest = floor(log2(m));
        std::string rest_code = number_code.substr(q + 1, size_of_rest);
        if (std::find(truncated_binary_codes.begin(), truncated_binary_codes.end(), rest_code ) == truncated_binary_codes.end() ) {
            size_of_rest++;
            rest_code = number_code.substr(q + 1, size_of_rest);
        }
        std::vector<std::string>::iterator itr = std::find(truncated_binary_codes.begin(), truncated_binary_codes.end(), rest_code);
        int r = std::distance(truncated_binary_codes.begin(), itr);

        // Create original number
        int number = m * q + r;

        if (number == 0) {
            // If number == 0, no bit for signal was passed on encoder
            return number;
        } else {
            // Check if the number is positive or negative.
            if (number_code[number_code.length() - 1] == '1') return -number;
            else return number;
        }

    }
    
    /*
        Function to decode a code using the BitStream to read bit a bit while decoding.
    */
    int decodeWithBitstream(BitStream& bitStream) {
		
        // Decode quotient part
        int q = 0;
        while (bitStream.get_bit() != 1) {
            q++;
        }

        // Decode truncated binary code
        int size_of_rest = floor(log2(m));
        std::string rest_code = bitStream.get_n_bits(size_of_rest);
        if (std::find(truncated_binary_codes.begin(), truncated_binary_codes.end(), rest_code ) == truncated_binary_codes.end()) {
            rest_code += std::to_string(bitStream.get_bit());
        }

        std::vector<std::string>::iterator itr = std::find(truncated_binary_codes.begin(), truncated_binary_codes.end(), rest_code);
        int r = std::distance(truncated_binary_codes.begin(), itr);

        // Create original number
        int number = m * q + r;

        if (number == 0) {
            // If number == 0, no bit for signal was passed on encoder
            return number;
        } else {
            // Check if the number is positive or negative. It requires to read 1 more bit.
            if (bitStream.get_bit() == 1) return -number;
            else return number;
        }

    }


    

};

#endif

