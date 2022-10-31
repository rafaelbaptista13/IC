#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <iostream>
#include <string>
#include <fstream>

class BitStream {
  private:
    std::ifstream myInputStream;
    std::ofstream myOutputStream;
    int bitReadCounter = 0;
    char byte = 0;
    std::string mode;
    int contador = 1;

  public:
	BitStream(std::string nomeFicheiro, std::string mode) {
      this->mode = mode;
      if (mode == "r") 
        this->myInputStream = std::ifstream(nomeFicheiro, std::ifstream::in);
      else
        this->myOutputStream = std::ofstream(nomeFicheiro, std::ofstream::out);
	}

	int get_bit() {
        
    int val;

    if (bitReadCounter % 8 == 0) {

        bitReadCounter = 0;
        byte = 0;

        if (!myInputStream.get(byte)) {
          return EOF;
        }
        
    }

    val = ((byte >> (7-bitReadCounter)) & 0x01);
    
    bitReadCounter++;

    return val;
	}


  std::string get_n_bits(int val) {
        
    std::string bits = "";

    for (int i = 0; i < val; i++) {
        int bit = get_bit();
        if (bit == 0) {
            bits += "0";
        } else if (bit == 1) {
            bits += "1";
        } else {
            return bits;
        }
    }
    contador++;

    return bits;
	}


  void write_bit(int val) {

    // Append the bit to the current byte
    byte = byte + (val << (7 - bitReadCounter));
    
    bitReadCounter++;
    
    if (bitReadCounter % 8 == 0) {
      // Full byte - ready to write in file
      bitReadCounter = 0;
      myOutputStream << byte;
      byte = 0;
      
    }

    return ;
  }


  void write_n_bits(std::string bits) {
    
    for (char const &bit: bits) {
        write_bit(bit - '0');
        //if (bits == "00000000000000000000000000001101") {
        //  std::cout << (int) byte << " " << bitReadCounter << " " << bit << std::endl;
          
        //}
    }
    myOutputStream.flush();
    //std::cout << "Já escrevi " << contador << std::endl;
    return ;
  }


  void close() {
    if (mode == "r")
      myInputStream.close();
    else
      if (bitReadCounter != 0) {
        // Write the remaining bits
        myOutputStream << byte;
        myOutputStream.close();
      }
  }

};




#endif

