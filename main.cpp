#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main()
{
    ifstream inFile("HandsOn-Input.txt");

    int no_problems, no_doctors;
    vector<pair<string, string>> cases;
    vector<pair<string, string>> doctors;
    vector<pair<string, string>> result;
    string name, speciality;
    
    inFile >> no_problems;

    for (int i = 0; i < no_problems; i++)
    {
        inFile >> name;
        inFile >> speciality;

        cases.emplace_back(name, speciality);
        cout << name << ' ' << speciality << '\n';
    }

    inFile >> no_doctors;

    for (int i = 0; i < no_doctors; i++)
    {
        inFile >> name;
        inFile >> speciality;

        doctors.emplace_back(name, speciality);
        cout << name << ' ' << speciality << '\n';
    }

    for (int i = 0; i < no_problems; i++) {
        auto problem = cases[i];

        for (int j = 0; j < doctors.size(); j++) {
            if (doctors[j].second == problem.second) {
                result.emplace_back(doctors[j].first, problem.first);
                doctors.erase(doctors.begin()+j );
                break;
            }
        }
    }

    for (int i = 0; i < result.size(); i++)
    {
        cout << result[i].first<< ' ' << result[i].second << '\n';
    }

    return 0;
}

