#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <set>
#include <unordered_map>

using namespace std;

class FileReader
{
private:
    ifstream inFile;

public: 
    void ReadFile(string fileName) {
        inFile = ifstream(fileName);

        

        inFile.close();
    }
};

class Solutions {
public : 
    void A() {}
    void B() {}
};

static string FILE_NAME = "Input.txt";

int main()
{
    FileReader fileReader = FileReader();
    Solutions solutions = Solutions();

    

    fileReader.ReadFile(FILE_NAME);

    solutions.A();
//    solutions.B();

    return 0;
}