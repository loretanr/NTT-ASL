#include <iostream>
#include <papi.h>

using namespace std;

struct node{
    int l, r;
};

void handle_error(int err){
    std::cerr << "PAPI error: " << err << std::endl;
}

int main(int argc, char* argv[]){

    int numEvents = 2;
    long long values[2];
    int events[2] = {PAPI_L3_TCA,PAPI_L3_TCM};

    if (PAPI_start_counters(events, numEvents) != PAPI_OK)
        handle_error(1);

    int n = 100000000;
    node* A = new node[n];
    int i;
    for(i=0;i<n;i++){
        A[i].l = 1;
        A[i].r = 4;
    }

    if ( PAPI_stop_counters(values, numEvents) != PAPI_OK)
        handle_error(1);

    cout<<"L3 accesses: "<<values[0]<<endl;
    cout<<"L3 misses: "<<values[1]<<endl;
    cout<<"L3 miss/access ratio: "<<(double)values[1]/values[0]<<endl;

    return 0;
}