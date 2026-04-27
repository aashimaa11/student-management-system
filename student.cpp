#include <bits/stdc++.h>
using namespace std;

// ===================== MODEL =====================
class Student {
public:
    int id;
    string name;
    int age;
    string course;
    vector<int> marks;
    float percentage;
    string grade;

    Student() {}

    Student(int id, string name, int age, string course, vector<int> marks) {
        this->id = id;
        this->name = name;
        this->age = age;
        this->course = course;
        this->marks = marks;
        calculateResult();
    }

    void calculateResult() {
        int sum = 0;
        for (int m : marks) sum += m;

        percentage = (float)sum / marks.size();

        if (percentage >= 90) grade = "A+";
        else if (percentage >= 75) grade = "A";
        else if (percentage >= 60) grade = "B";
        else if (percentage >= 50) grade = "C";
        else grade = "Fail";
    }

    string serialize() {
        string m = "";
        for (int x : marks) m += to_string(x) + "|";

        return to_string(id) + "," + name + "," + to_string(age) + "," +
               course + "," + m;
    }

    static Student deserialize(string line) {
        stringstream ss(line);
        string token;
        vector<string> data;

        while (getline(ss, token, ',')) data.push_back(token);

        vector<int> marks;
        stringstream ms(data[4]);
        string t;

        while (getline(ms, t, '|')) {
            if (!t.empty()) marks.push_back(stoi(t));
        }

        return Student(stoi(data[0]), data[1], stoi(data[2]), data[3], marks);
    }
};

// ===================== SERVICE =====================
class StudentService {
private:
    vector<Student> students;
    unordered_map<int, int> indexMap;
    stack<Student> deletedStack;
    string filename = "students.txt";

    void loadFromFile() {
        students.clear();
        indexMap.clear();

        ifstream file(filename);
        string line;

        while (getline(file, line)) {
            if (!line.empty()) {
                Student s = Student::deserialize(line);
                students.push_back(s);
                indexMap[s.id] = students.size() - 1;
            }
        }
        file.close();
    }

    void saveToFile() {
        ofstream file(filename);
        for (auto &s : students) {
            file << s.serialize() << endl;
        }
        file.close();
    }

public:
    StudentService() {
        loadFromFile();
    }

    // ================= ADD =================
    void addStudent() {
        int id, age, n;
        string name, course;

        cout << "Enter ID: ";
        cin >> id;

        if (indexMap.count(id)) {
            cout << " ID exists!\n";
            return;
        }

        cin.ignore();
        cout << "Enter Name: ";
        getline(cin, name);

        cout << "Enter Age: ";
        cin >> age;
        cin.ignore();

        cout << "Enter Course: ";
        getline(cin, course);

        cout << "Subjects count: ";
        cin >> n;

        vector<int> marks(n);
        for (int i = 0; i < n; i++) {
            cout << "Marks " << i + 1 << ": ";
            cin >> marks[i];
        }

        Student s(id, name, age, course, marks);
        students.push_back(s);
        indexMap[id] = students.size() - 1;

        saveToFile();
        cout << " Added!\n";
    }

    // ================= VIEW =================
    void viewStudents() {
        if (students.empty()) {
            cout << "No data!\n";
            return;
        }

        cout << "\nID | Name | % | Grade\n";
        for (auto &s : students) {
            cout << s.id << " | " << s.name
                 << " | " << s.percentage << "% | "
                 << s.grade << endl;
        }
    }

    // ================= SEARCH =================
    void searchById() {
        int id;
        cout << "Enter ID: ";
        cin >> id;

        if (!indexMap.count(id)) {
            cout << " Not found!\n";
            return;
        }

        auto &s = students[indexMap[id]];
        cout << s.name << " | " << s.percentage << "% | " << s.grade << endl;
    }

    void searchByName() {
        string name;
        cin.ignore();
        cout << "Enter Name: ";
        getline(cin, name);

        bool found = false;
        for (auto &s : students) {
            if (s.name == name) {
                cout << s.id << " | " << s.percentage << "%\n";
                found = true;
            }
        }
        if (!found) cout << " Not found\n";
    }

    void searchByGrade() {
        string g;
        cout << "Enter Grade: ";
        cin >> g;

        for (auto &s : students) {
            if (s.grade == g) {
                cout << s.name << " | " << s.percentage << "%\n";
            }
        }
    }

    // ================= UPDATE =================
    void updateStudent() {
        int id;
        cout << "Enter ID: ";
        cin >> id;

        if (!indexMap.count(id)) {
            cout << " Not found!\n";
            return;
        }

        int idx = indexMap[id];

        cin.ignore();
        cout << "New Name: ";
        getline(cin, students[idx].name);

        cout << "New Age: ";
        cin >> students[idx].age;

        int n;
        cout << "Subjects count: ";
        cin >> n;

        students[idx].marks.clear();
        for (int i = 0; i < n; i++) {
            int x;
            cout << "Marks " << i + 1 << ": ";
            cin >> x;
            students[idx].marks.push_back(x);
        }

        students[idx].calculateResult();

        saveToFile();
        cout << " Updated!\n";
    }

    // ================= DELETE =================
    void deleteStudent() {
        int id;
        cout << "Enter ID: ";
        cin >> id;

        if (!indexMap.count(id)) {
            cout << "Not found!\n";
            return;
        }

        int idx = indexMap[id];
        deletedStack.push(students[idx]);

        swap(students[idx], students.back());
        indexMap[students[idx].id] = idx;

        students.pop_back();
        indexMap.erase(id);

        saveToFile();
        cout << "Deleted!\n";
    }

    // ================= UNDO =================
    void undoDelete() {
        if (deletedStack.empty()) {
            cout << "Nothing to undo!\n";
            return;
        }

        Student s = deletedStack.top();
        deletedStack.pop();

        students.push_back(s);
        indexMap[s.id] = students.size() - 1;

        saveToFile();
        cout << "↩ Undo Done!\n";
    }

    // ================= SORT =================
    void sortByName() {
        sort(students.begin(), students.end(),
             [](Student a, Student b) { return a.name < b.name; });

        for (int i = 0; i < students.size(); i++)
            indexMap[students[i].id] = i;

        cout << "Sorted by Name!\n";
    }

    void sortByPercentage() {
        sort(students.begin(), students.end(),
             [](Student a, Student b) { return a.percentage > b.percentage; });

        for (int i = 0; i < students.size(); i++)
            indexMap[students[i].id] = i;

        cout << "Sorted by %!\n";
    }

    // ================= TOPPER =================
    void showTopper() {
        if (students.empty()) return;

        auto t = max_element(students.begin(), students.end(),
                             [](Student a, Student b) {
                                 return a.percentage < b.percentage;
                             });

        cout << "🏆 Topper: " << t->name << " (" << t->percentage << "%)\n";
    }

    // ================= ANALYTICS =================
    void showAnalytics() {
        if (students.empty()) return;

        float total = 0;
        int fail = 0;

        for (auto &s : students) {
            total += s.percentage;
            if (s.grade == "Fail") fail++;
        }

        cout << " Avg: " << total / students.size() << endl;
        cout << " Fail Count: " << fail << endl;
    }
};

// ================= LOGIN =================
bool login() {
    string u, p;
    cout << "Username: ";
    cin >> u;
    cout << "Password: ";
    cin >> p;

    if (u == "admin" && p == "1234") {
        cout << " Login Success\n";
        return true;
    }

    cout << " Wrong Credentials\n";
    return false;
}

// ================= MAIN =================
int main() {
    if (!login()) return 0;

    StudentService s;
    int ch;

    while (true) {
        cout << "\n===== PRO MENU =====\n";
        cout << "1.Add\n2.View\n3.Search ID\n4.Update\n5.Delete\n6.Undo\n";
        cout << "7.Sort Name\n8.Sort %\n9.Topper\n10.Search Name\n";
        cout << "11.Search Grade\n12.Analytics\n13.Exit\n";

        cout << "Choice: ";
        cin >> ch;

        switch (ch) {
        case 1: s.addStudent(); break;
        case 2: s.viewStudents(); break;
        case 3: s.searchById(); break;
        case 4: s.updateStudent(); break;
        case 5: s.deleteStudent(); break;
        case 6: s.undoDelete(); break;
        case 7: s.sortByName(); break;
        case 8: s.sortByPercentage(); break;
        case 9: s.showTopper(); break;
        case 10: s.searchByName(); break;
        case 11: s.searchByGrade(); break;
        case 12: s.showAnalytics(); break;
        case 13: return 0;
        default: cout << "Invalid!\n";
        }
    }
}