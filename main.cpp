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
};

struct Doctor {
    string name;
    set<string> specialities;
    vector<Case> handeledCases;
    int remainingTime = 8;
};

int main()
{
    ifstream inFile("HandsOn-Input.txt");

    int no_problems, no_doctors;
    string name, speciality;
    int priority, duration, arrival_time, no_specialities;

    vector<Case> cases;
    map<string, Doctor> doctors;
    map<string, vector<string>> doctorsBySpecialities;
    
    inFile >> no_problems;

    for (int i = 0; i < no_problems; i++)
    {
        inFile >> name;
        inFile >> speciality;
        inFile >> arrival_time;
        inFile >> duration;
        inFile >> priority;

        cases.emplace_back(Case(name, speciality, arrival_time, priority, duration));
        cout << name << ' ' << speciality << ' ' << arrival_time << ' ' << priority << ' ' << duration << '\n';
    }

    cout << endl;
    inFile >> no_doctors;

    for (int i = 0; i < no_doctors; i++)
    {
        Doctor current_doctor = Doctor();
        inFile >> name;
        inFile >> no_specialities;
        current_doctor.name = name;
        cout << name << ' ' << no_specialities << ' ';

        for (int i = 0; i < no_specialities; i++) {
            inFile >> speciality;
            cout << speciality << '\n';
            
            current_doctor.specialities.insert(speciality);
            doctorsBySpecialities[speciality].emplace_back(current_doctor.name);

        }
        doctors.insert({ current_doctor.name, current_doctor });
    }

    sort(cases.begin(), cases.end(), [](const Case& c1, const Case& c2) {
        if (c1.arrival_time < c2.arrival_time) 
            return true;
        if (c1.arrival_time > c2.arrival_time)
            return false;
        
        return c1.priority > c2.priority;
    });

    for (Case& currentCase : cases) {

        // find the first [free] doctor, if there is any
        // use the map containing doctors by speciality to find a specialist
            // use the map containg the doctors to quickly acces the doctor
        for (string& currentDoctorId : doctorsBySpecialities[currentCase.speciality]) {
            Doctor& currentDoctor = doctors[currentDoctorId];
            // Verificam la ce ora s-a terminat cazul anterior, daca exista.
            if (currentDoctor.handeledCases.size() > 0) {
                if (currentDoctor.handeledCases[currentDoctor.handeledCases.size() - 1].arrival_time +
                    currentDoctor.handeledCases[currentDoctor.handeledCases.size() - 1].duration
                        > currentCase.arrival_time) {
                    continue;
                }
            }

            if (currentDoctor.remainingTime - currentCase.duration >= 0) {
                currentDoctor.remainingTime -= currentCase.duration;
                currentDoctor.handeledCases.push_back(currentCase);
                break;
            }
        }
    }

    // display
    for (auto& currentDoctor : doctors) {
        if (currentDoctor.second.handeledCases.size() == 0)
            continue;

        cout << currentDoctor.second.name << " " << currentDoctor.second.handeledCases.size() ;
        for (Case currentCase : currentDoctor.second.handeledCases) {
            cout << " " << currentCase.name << " " << currentCase.arrival_time;
        }
        cout << endl;
    }
   

    return 0;
}