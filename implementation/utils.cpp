#include "utils.h" // Defines MOD_xx operations
#include <iostream>
#include <fstream>

using namespace std;

ull modpow(ull base, ull exp, ull modulus)
{
    ull result = 1;
    while (exp > 0)
    {
        if (exp & 1)
            result = (result * base) % modulus;
        base = (base * base) % modulus;
        exp >>= 1;
    }
    return result;
}

// dummy function for those transforms that dont need init
void dummy_init(ull s, ull t, ull m){}

void readFromFiles(std::string infile, unsigned *vec)
{
    std::ifstream instream;
    instream.open(infile, ios::in);
    if (!instream.fail())
    {
        unsigned x;
        int i = 0; // Index used for input vector
        while (instream)
        {
            instream >> x;     // Read number
            instream.ignore(); // Ignore comma
            vec[i] = x;        // Load number into input vector
            i++;
        }
    }
    else { // File failed to open
        cout << "File " << infile << " failed to open" << endl;
    }
}