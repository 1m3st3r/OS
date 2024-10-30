#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

// Reporter
int reporter(string inputFile, string outputFile, double hourlyRate) {
    ifstream infile(inputFile, ios::binary);

    if (!infile.is_open()) {
        cerr << "Error open" << endl;
        return 1;
    }

    ofstream outfile(outputFile);

    if (!outfile.is_open()) {
        cerr << "Error open" << endl;
        return 1;
    }

    outfile << "file report: " << inputFile << endl;
    outfile << "Num: Employee name: Time: Salary:" << endl;

    employee emp;

    while (infile.read((char*)&emp, sizeof(emp))) {
        double salary = emp.hours * hourlyRate;
        outfile << emp.num << " |===| " << emp.name << " |===| " << emp.hours << " |===| " << salary << endl;
    }

    infile.close();
    outfile.close();

    return 0;
}

// Creator
int creator(string filename, int count) {
    ofstream outfile(filename, ios::binary);

    if (!outfile.is_open()) {
        cerr << "Error open" << endl;
        return 1;
    }

    for (int i = 0; i < count; i++) {
        employee emp;
        cout << "Enter number of employee: ";
        cin >> emp.num;
        cout << "Enter name of employee: ";
        cin >> emp.name;
        cout << "Hours of work: ";
        cin >> emp.hours;

        outfile.write((char*)&emp, sizeof(emp));
    }

    outfile.close();

    return 0;
}


int main() {
    setlocale(LC_ALL, "ru");
    string filename;
    int count;

    cout << "Enter the name of bin file: ";
    cin >> filename;
    cout << "Enter the number of records: ";
    cin >> count;

    if (creator(filename, count) != 0) {
        return 1;
    }

    string reportFilename;
    double hourlyRate;
    cout << "Enter the name of report bin file: ";
    cin >> reportFilename;
    cout << "Enter the pay for hour: ";
    cin >> hourlyRate;

    if (reporter(filename, reportFilename, hourlyRate) != 0) {
        return 1;
    }

    ifstream outfile(reportFilename);
    if (!outfile.is_open()) {
        cerr << "Enter open" << endl;
        return 1;
    }

    string str;
    while (getline(outfile, str)) {
        cout << str << endl;
    }

    outfile.close();

    return 0;
}
