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

        ofs.close();
    } else {
        ifstream ifs { argv[argc-2] };
        int val;

        while (ifs >> val) {
            cout << val << endl;
            bitStream.write_bit(val);
        }

        ifs.close();
    }

    bitStream.close();

    
}