#ifndef GOLOMB_CODEC_H
#define GOLOMB_CODEC_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <bitset>
#include <algorithm>
#include "BitStream.h"

class GOLOMBCodec {
  private:
	int m;          // Divsion Factor
    std::vector<std::string> truncated_binary_codes;

  public:
	GOLOMBCodec(int m) {
		this->m = m;

        int min_num_symbols_per_code = floor(log2(m));
        int max_num_symbols_per_code = min_num_symbols_per_code + 1;

        int num_codes_with_min_num_symbols_per_code =  pow(2, min_num_symbols_per_code) - (m - pow(2, min_num_symbols_per_code));
        int num_codes_with_max_num_symbols_per_code = m - num_codes_with_min_num_symbols_per_code;

        std::cout << "max" << " : " << num_codes_with_max_num_symbols_per_code << std::endl;
        std::cout << "min" << " : " << num_codes_with_min_num_symbols_per_code << std::endl;

        std::string symbolcode;
        
        for (int i = 0; i < num_codes_with_min_num_symbols_per_code; i++ ) {
            symbolcode = std::bitset<32>(i).to_string();
            symbolcode = symbolcode.substr(symbolcode.length() - min_num_symbols_per_code, symbolcode.length());
            truncated_binary_codes.push_back(symbolcode);
        }

        if (num_codes_with_max_num_symbols_per_code > 0) {
            // Add a 0 to the end of the binary code
            int last_num = num_codes_with_min_num_symbols_per_code << 1;    

            // Convert to binary string
            std::string code_first_symbol_with_max = std::bitset<32>( last_num ).to_string();
            code_first_symbol_with_max = code_first_symbol_with_max.substr(code_first_symbol_with_max.length() - max_num_symbols_per_code, code_first_symbol_with_max.length());
            
            truncated_binary_codes.push_back(code_first_symbol_with_max);
            std::cout << last_num << std::endl;

            for (int i = num_codes_with_min_num_symbols_per_code+1; i<m; i++) {
                int current_num = last_num + 1;
                symbolcode = std::bitset<32>(current_num).to_string();
                symbolcode = symbolcode.substr(symbolcode.length() - max_num_symbols_per_code, symbolcode.length());
                truncated_binary_codes.push_back(symbolcode);

                last_num = current_num;
            }
        
        }


        /*
            ESTE CICLO FOR É SO PARA PRINT. PODE SER TIRADO DEPOIS
        */
        for (auto it = truncated_binary_codes.begin(); it != truncated_binary_codes.end(); ++it) {
            int index = std::distance(truncated_binary_codes.begin(), it);
            std::cout << index << " : " << truncated_binary_codes[index] << std::endl;
        }
        

	}

	std::string encode(int number) {
		
        std::string code;
        int q, r;
        q = abs(number / m);
        r = abs(number % m);

        code = encodeQuotient(q) + truncated_binary_codes[r];

        if (number > 0) code += "0";
        else if (number < 0) code += "1";

        return code;
    }


    int decode(std::string number_code) {
		int q = 0;
        while (number_code[q] != '1') {
            q++;
        }
        int size_of_rest = floor(log2(m));
        std::string rest_code = number_code.substr(q + 1, size_of_rest);
        while (std::find(truncated_binary_codes.begin(), truncated_binary_codes.end(), rest_code ) == truncated_binary_codes.end() ) {
            size_of_rest++;
            rest_code = number_code.substr(q + 1, size_of_rest);
        }
        std::vector<std::string>::iterator itr = std::find(truncated_binary_codes.begin(), truncated_binary_codes.end(), rest_code);
        int r = std::distance(truncated_binary_codes.begin(), itr);

        int number = m * q + r;

        if (number == 0) {
            return number;
        } else {
            if (number_code[number_code.length() - 1] == '1') return -number;
            else return number;
        }

    }
    int decodeWithBitstream(BitStream& bitStream) {
		int q = 0;

        while (bitStream.get_bit() != 1) {
            q++;
        }
        int size_of_rest = floor(log2(m));

        std::string rest_code = bitStream.get_n_bits(size_of_rest);
        while (std::find(truncated_binary_codes.begin(), truncated_binary_codes.end(), rest_code ) == truncated_binary_codes.end() ) {
            size_of_rest++;
            rest_code += std::to_string(bitStream.get_bit());
        }
        std::vector<std::string>::iterator itr = std::find(truncated_binary_codes.begin(), truncated_binary_codes.end(), rest_code);
        int r = std::distance(truncated_binary_codes.begin(), itr);

        int number = m * q + r;

        if (number == 0) {
            return number;
        } else {
            if (bitStream.get_bit() == 1) return -number;
            else return number;
        }

    }


    std::string encodeQuotient(int quotient) {

        std::string code { "" };

        for (int i = 0; i < quotient; i++) {
            code+="0";
        }
        code += "1";

        return code;
    }

};

#endif

/*
0 - 000
1 - 001
2 - 010
3 - 011
4 - 100
5 - 101
6 - 1100
7 - 1101
8 - 1110
9 - 1111*/