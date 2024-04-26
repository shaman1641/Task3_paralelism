#include <iostream>
#include <string>
#include <cmath>
#include <fstream> 
#include <vector>
#include <cstring>

using namespace std;

int main(){ 
    const char* const delimeters = " ";

    std::ofstream filet("tester.txt");
    string s;
    float epsi = 0.001;

    std::ofstream fileerr("tester_error.txt");
    int errors = 0;

    std::ifstream file1("t1.txt");

    while(getline(file1, s)){ 
        filet << s << endl;
        std::vector<std::string> words;
        char* token = std::strtok(s.data(), delimeters);        
        while (token != nullptr)
        {         
            words.push_back(token);            
            token = std::strtok(nullptr, delimeters);               
        }
        if(sin(stof(words[1])) - stof(words[2]) > epsi ){
            filet << "ERROR SIN = " << sin(stof(words[1]))  << endl;
            fileerr << "ERROR SIN = " << sin(stof(words[1]))  << endl;
            fileerr << "FILE WALUE = " << stof(words[2]) << endl;
            errors++;
        }
        else{
            filet << "PASS" << endl;
        }
    }

    file1.close();

    std::ifstream file2("t2.txt");

    while(getline(file2, s)){ 
        filet << s << endl;
        std::vector<std::string> words;
        char* token = std::strtok(s.data(), delimeters);        
        while (token != nullptr)
        {         
            words.push_back(token);            
            token = std::strtok(nullptr, delimeters);               
        }
        if(sqrt(stof(words[1])) - stof(words[2]) > epsi){
            filet << "ERROR SQRT = " << sqrt(stof(words[1]))  << endl;
            fileerr << "ERROR SQRT = " << sqrt(stof(words[1]))  << endl;
            fileerr << "FILE WALUE = " << stof(words[2]) << endl;
            errors++;
        }
        else{
            filet << "PASS" << endl;
        }
    }

    file2.close();

    std::ifstream file3("t3.txt");

    while(getline(file3, s)){ 
        filet << s << endl;
        std::vector<std::string> words;
        char* token = std::strtok(s.data(), delimeters);        
        while (token != nullptr)
        {         
            words.push_back(token);            
            token = std::strtok(nullptr, delimeters);               
        }
        if(pow(stof(words[1]),2.0) - stof(words[2]) > epsi){
            filet << "ERROR POW = " << pow(stof(words[1]),2.0)  << endl;
            fileerr << "ERROR POW = " << pow(stof(words[1]),2.0)  << endl;
            fileerr << "FILE WALUE = " << stof(words[2]) << endl;
            errors++;
        }
        else{
            filet << "PASS" << endl;
        }
    }

    file3.close();

    cout << "ERROR: " << errors  << endl;

    return 0;
}