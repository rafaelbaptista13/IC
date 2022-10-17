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

        if (!(myInputStream >> byte)) {
          return EOF;
        }

      

    }

    val = ((byte >> (7-bitReadCounter)) & 0x01);
    
    bitReadCounter++;

    return val;
	}

  void write_bit(int val) {

    byte = byte + (val << (7 - bitReadCounter));
    
    bitReadCounter++;
    if (bitReadCounter % 8 == 0) {
      
      bitReadCounter = 0;
      myOutputStream << byte;
      byte = 0;

    }

    return ;
  }


  void close() {
    if (mode == "r")
      myInputStream.close();
    else
      if (bitReadCounter != 0) {

        // FALTA VER SE FALTA ALGUMA COISA
        myOutputStream << byte;
        myOutputStream.close();
      }
  }

};




#endif

