#include <iostream>

using namespace std;

struct node{
    int l, r;
};

int main(int argc, char* argv[]){

    int n = 100000000;

    node* A = new node[n];

    int i;
    for(i=0;i<n;i++){
        A[i].l = 1;
        A[i].r = 4;
    }

    return 0;
}