#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
using namespace std;


// use unsigned int or size_t
int reverse(string& s, int start, int end)
{
    for(int i=start; i<=(start + end)/2; i++)
    {
        char c = s[i];
        s[i] = s[start + end - i];
        s[start + end - i] = c;
    }
    return 0;
}

void print_string(string& s)
{
    for (const auto& c : s) {
        cout << c;
    }
    cout << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "usage:" << argv[0] << ":" << "<some string to reverse>" << endl;
        exit(0);
    }
    string s = argv[1];
    reverse(s, 0, s.size()-1);
    print_string(s);
    unsigned int start_space = 0;

    for (unsigned int i=0; i<s.size(); i++)
    {
        if(s[i] == ' ')
        {
            reverse(s, start_space, i-1);
            start_space = i + 1;
        }
    }
    reverse(s, start_space, s.size()-1);

    print_string(s);

    cout << endl;
}

