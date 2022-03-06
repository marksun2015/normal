#include "numberchains.h"
#include <iostream>

using namespace std;

int main() {
    string input_str;
    cout << "please input number: " << endl;
    cin >> input_str;
    cout << endl;
    //cout << "input " << atoi(input_str.c_str()) << endl;
    //unique_ptr<NumberChains> ns = make_unique<NumberChains>(atoi(input_str.c_str()));
    auto ns = make_unique<NumberChains>(atoi(input_str.c_str()));
    return 0;
}
