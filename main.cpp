#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct Case {
    string name, speciality;
    int priority, duration;
    bool reviewd = false;
};

struct Doctor {
    string name, speciality;
    std::queue<Case> queue;
    int remainingTime = 8;
};

void matchSpeciality(Case& currentCase, vector<Doctor>& doctors) {
    for_each(doctors.begin(), doctors.end(), [&currentCase](auto& currentDoctor){
        if (currentDoctor.speciality == currentCase.speciality && currentDoctor.remainingTime - currentCase.duration >= 0 &&
            currentCase.reviewd == false) {
            currentDoctor.queue.push(currentCase);
            currentDoctor.remainingTime -= currentCase.duration;
            currentCase.reviewd = true;
        }
    });
}

int main()
{
    ifstream inFile("HandsOn-Input.txt");

    int no_problems, no_doctors;
    vector<Case> cases;
    vector<Doctor> doctors;
    string name, speciality;
    int priority, duration;
    
    inFile >> no_problems;

    for (int i = 0; i < no_problems; i++)
    {
        inFile >> name;
        inFile >> speciality;
        inFile >> duration;
        inFile >> priority;

        cases.emplace_back(name, speciality, priority, duration);
        cout << name << ' ' << speciality << ' ' << priority << ' ' << duration << '\n';
    }

    cout << endl;
    inFile >> no_doctors;

    for (int i = 0; i < no_doctors; i++)
    {
        inFile >> name;
        inFile >> speciality;

        doctors.emplace_back(name, speciality);
        cout << name << ' ' << speciality << '\n';
    }

    sort(cases.begin(), cases.end(), [](const Case& c1, const Case& c2) {
        return c1.priority > c2.priority;
    });

    cout << "After input -------------------" << endl;

    std::for_each(cases.begin(), cases.end(), [&doctors](Case& currentCase) {
        matchSpeciality(currentCase, doctors);
    });
    
    // display
    for_each(doctors.begin(), doctors.end(), [](auto& currentDoctor) {
        if (!currentDoctor.queue.empty()) {
            cout << currentDoctor.name << " " << currentDoctor.queue.size() << " ";
            while (!currentDoctor.queue.empty()) {
                auto currentCase = currentDoctor.queue.front();
                currentDoctor.queue.pop();
                cout << currentCase.name << " ";
            }
            cout << endl;
        };
    });

        

    return 0;
}