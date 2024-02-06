#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>
#include <map>

using namespace std;

struct Case {
    string name, speciality;
    int arrival_time, priority, duration;
    bool reviewd = false;
};

struct Doctor {
    string name;
    set<string> specialities;
    queue<Case> queue;
    int remainingTime = 8;
};

struct Staff {
    // specialitati - doctori
    map<string, vector<Doctor>> doctorsBySpecialities;
};

void matchSpeciality(Case& currentCase, vector<Doctor>& doctors, Staff staff) {
    /*for_each(doctors.begin(), doctors.end(), [&currentCase](auto& currentDoctor){
        if (currentDoctor.specialities.contains(currentCase.speciality) &&
            currentDoctor.remainingTime - currentCase.duration >= 0 &&
            currentCase.reviewd == false) 
        {
            currentDoctor.queue.push(currentCase);
            currentDoctor.remainingTime -= currentCase.duration;
            currentCase.reviewd = true;
        }
    });*/
    
    for (auto doctor : staff.doctorsBySpecialities[currentCase.speciality]) {
        if (doctor.remainingTime - currentCase.duration >= 0 &&
            currentCase.reviewd == false)
        {
            doctor.queue.push(currentCase);
            doctor.remainingTime -= currentCase.duration;
            currentCase.reviewd = true;
        }
    }

}

int main()
{
    ifstream inFile("HandsOn-Input.txt");

    int no_problems, no_doctors;
    vector<Case> cases;
    vector<Doctor> doctors;
    Staff staff;
    string name, speciality;
    set<string> specialities;
    int priority, duration, arrival_time, no_specialities;
    
    inFile >> no_problems;

    for (int i = 0; i < no_problems; i++)
    {
        inFile >> name;
        inFile >> speciality;
        inFile >> arrival_time;
        inFile >> duration;
        inFile >> priority;

        cases.emplace_back(name, speciality, priority, duration);
        cout << name << ' ' << speciality << ' ' << arrival_time << ' ' << priority << ' ' << duration << '\n';
    }

    cout << endl;
    inFile >> no_doctors;

    for (int i = 0; i < no_doctors; i++)
    {
        auto current_doctor = Doctor();
        inFile >> name;
        inFile >> no_specialities;
        cout << name << ' ' << no_specialities << ' ';
        
        current_doctor.name = name;
        for (int i = 0; i < no_specialities; i++) {
            inFile >> speciality;
            current_doctor.specialities.insert(speciality);
            specialities.insert(speciality);
            staff.doctorsBySpecialities[speciality].emplace_back(current_doctor);
            cout << speciality << '\n';
        }

        doctors.emplace_back(current_doctor);
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