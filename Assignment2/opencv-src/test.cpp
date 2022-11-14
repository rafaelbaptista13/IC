#include <iostream>
#include "golomb_codec.h"

using namespace std;

int main(int argc,const char** argv) {

   
    GOLOMBCodec codec {11};
    
    string encoded_number = codec.encode(-1);
    cout << encoded_number << endl;
    int number = codec.decode(encoded_number);
    cout << number << endl;

    return 0;
}

