#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Case {
    string name, speciality;
};

struct Doctor {
    string name, speciality;
    bool inUse = false;
};

struct Result {
    string name, speciality;
};

bool matchSpeciality(const Doctor& doctor, const Case& currentCase) {
    return doctor.speciality == currentCase.speciality && doctor.inUse == false;
}

int main()
{
    ifstream inFile("HandsOn-Input.txt");

    int no_problems, no_doctors;
    vector<Case> cases;
    vector<Doctor> doctors;
    vector<Result> result;
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

    for (int i = 0; i < cases.size(); i++) {
        auto currentCase = cases[i];
        auto doctorIterator = find_if(doctors.begin(), doctors.end(), [currentCase](const Doctor& dr) {
            return matchSpeciality(dr, currentCase);
        });
        if (doctorIterator != doctors.end()) {
            (*doctorIterator).inUse = true;
            Doctor doctor = *doctorIterator;
            result.push_back(Result{ doctor.name, currentCase.name });
        }
    }

    for (auto currentResult : result)
        cout << currentResult.name<< ' ' << currentResult.speciality << '\n';

    return 0;
}