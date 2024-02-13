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

struct Case {
    string name, speciality;
    int arrival_time, priority, duration;

    Case(const std::string& name, const std::string& speciality,
        int arrival_time, int priority, int duration)
        : name(name), speciality(speciality),
        arrival_time(arrival_time), priority(priority),
        duration(duration) {}

    Case(const Case& other)
        : name(other.name), speciality(other.speciality),
        arrival_time(other.arrival_time), priority(other.priority),
        duration(other.duration) {}

    bool operator<(const Case& other) const {
        if (arrival_time < other.arrival_time)
            return true;
        if (arrival_time > other.arrival_time)
            return false;
        return priority > other.priority;
    }

    bool operator==(const Case& other) const {
        return name == other.name &&
            speciality == other.speciality &&
            arrival_time == other.arrival_time &&
            priority == other.priority &&
            duration == other.duration;
    }
};

namespace std {
    template<> struct hash<Case> {
        size_t operator()(const Case& c) const {
            return hash<std::string>()(c.name) ^
                hash<std::string>()(c.speciality) ^
                hash<int>()(c.arrival_time) ^
                (hash<int>()(c.priority) << 2) ^
                (hash<int>()(c.duration) << 1);
        }
    };
}

struct Doctor {
    string name;
    unordered_set<string> specialities;
    set<Case> handeledCases;
    int remainingTime = 8;
};

class FileReader
{
private:
    ifstream inFile;

    void ReadCasesFromFile(set<Case>& cases, string fileName) {
        int no_problems, no_doctors;
        string name, speciality;
        int priority, duration, arrival_time, no_specialities;

        inFile >> no_problems;

        for (int i = 0; i < no_problems; i++)
        {
            inFile >> name;
            inFile >> speciality;
            inFile >> arrival_time;
            inFile >> duration;
            inFile >> priority;

            cases.insert(Case(name, speciality, arrival_time, priority, duration));
            cout << name << ' ' << speciality << ' ' << arrival_time << ' ' << priority << ' ' << duration << '\n';
        }
        cout << endl;
    }
    
    void ReadDoctorsFromFile(
        unordered_map<string, Doctor>& doctors, unordered_map<string, vector<string>>& doctorsBySpecialities) {
        string name, speciality;
        int no_doctors, no_specialities;

        

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
    }

public: 
    void ReadFile(string fileName, set<Case>& cases,
                    unordered_map<string, Doctor>& doctors, unordered_map<string, vector<string>>& doctorsBySpecialities) {
        inFile = ifstream(fileName);

        ReadCasesFromFile(cases, fileName);

        ReadDoctorsFromFile(doctors, doctorsBySpecialities);

        inFile.close();
    }
};

int main()
{
    string fileName = "HandsOn-Input.txt";
    set<Case> cases;
    unordered_map<string, Doctor> doctors;
    unordered_map<string, vector<string>> doctorsBySpecialities;
    
    FileReader file = FileReader();
    file.ReadFile(fileName, cases, doctors, doctorsBySpecialities);

    cout << "After output -----------------------" << endl;

    for (const Case& currentCase : cases) {

        for (string& currentDoctorId : doctorsBySpecialities[currentCase.speciality]) {
            Doctor& currentDoctor = doctors[currentDoctorId];
            if (currentDoctor.handeledCases.size() > 0) {
                auto lastElementIterator = currentDoctor.handeledCases.rbegin();
                if ((lastElementIterator->arrival_time + lastElementIterator->duration) > currentCase.arrival_time) {
                    continue;
                }
            }

            if (currentDoctor.remainingTime - currentCase.duration >= 0) {
                currentDoctor.remainingTime -= currentCase.duration;
                currentDoctor.handeledCases.insert(currentCase);
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