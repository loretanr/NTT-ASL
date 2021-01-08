#include <iostream>

using namespace std;


int main(){
    int prime = 41;
    int gen = 37;
    int curr = 0;

    int *nums = new int[prime];

    for(int i=0; i<prime; i++){
        curr += gen;
        curr %= prime;
        nums[i] = curr;
        cout << curr << " ";
    } cout << endl;
}