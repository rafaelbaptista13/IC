#include <iostream>
#include <fstream>
#include "BitStream.h"

using namespace std;

int main(int argc, char *argv[]) {

    string mode { "r" };

    if (argc < 2 ) {
        cerr << "test_bs [ -r (def) ]\n";
        cerr << "        [ -w ]\n";
        cerr << "        text_file bin_file\n";
        return 1;
    }

    for (int n=0; n<argc; n++) {
        if (string(argv[n]) == "-r") {
            mode = "r";
            break;
        }

        if (string(argv[n]) == "-w") {
            mode = "w";
            break;
        }
    }
    cout << mode << endl;
    BitStream bitStream { argv[argc-1], mode };

    if (mode == "r") {
        ofstream ofs { argv[argc-2] };
        int bit;

        while ( (bit = bitStream.get_bit()) != EOF )
            ofs << bit;
        
        /* Version to use get_n_bits 
        string bits;
        while ( (bits = bitStream.get_n_bits(3) ) != "" ) {
            for (char const &bit: bits) {
                ofs << bit - '0';
            }
        }
        */

        ofs.close();
    } else {
        ifstream ifs { argv[argc-2] };
        char val;
        
        while (ifs.get(val)) {
            for (int i = 7; i >= 0; i--) 
                bitStream.write_bit(((val >> i) & 1));
        }
        
        /* Version to use write_n_bits
        while (ifs.get(val)) {
            string bits = "";
            for (int i = 7; i >= 0; i--) {
                int bit = ((val >> i) & 1);
                bits += to_string(bit);
            }
            bitStream.write_n_bits(bits);
        }
        */

        /* Not working but it's the professor version 
        int val;
        while (ifs >> val) {
            bitStream.write_bit(val);
        }
        */

        ifs.close();
    }

    bitStream.close();

    
}