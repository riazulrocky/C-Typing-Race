#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace std::chrono;

struct User {
    string username;
    string password;
    double bestWPM = 0.0;
    double bestAccuracy = 0.0;
    int attempts = 0;
    double totalWPM = 0.0;
};

vector<User> users;

// ANSI color codes
const string GREEN = "\033[32m";
const string RED = "\033[31m";
const string RESET = "\033[0m";
const string GOLD = "\033[93m";
const string SILVER = "\033[37m";
const string BRONZE = "\033[33m";

// Pause until Enter pressed
void pressToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

// Load users from file
void loadUsers() {
    ifstream file("users.txt");
    if (!file) return;
    User u;
    while(file >> u.username >> u.password >> u.bestWPM >> u.bestAccuracy >> u.attempts >> u.totalWPM) {
        users.push_back(u);
    }
    file.close();
}

// Save users to file
void saveUsers() {
    ofstream file("users.txt");
    for(auto &u: users) {
        file << u.username << " " << u.password << " " << u.bestWPM << " "
             << u.bestAccuracy << " " << u.attempts << " " << u.totalWPM << endl;
    }
    file.close();
}

// Register new user
bool registerUser(string username, string password) {
    for(auto &u: users) {
        if(u.username == username) return false; // already exists
    }
    User newUser{username, password, 0.0, 0.0, 0, 0.0};
    users.push_back(newUser);
    saveUsers();
    return true;
}

// Login
User* loginUser(string username, string password) {
    for(auto &u: users) {
        if(u.username == username && u.password == password) return &u;
    }
    return nullptr;
}

// Leaderboard
void showLeaderboard() {
    vector<User> leaderboard = users;
    sort(leaderboard.begin(), leaderboard.end(), [](User &a, User &b) {
        return a.bestWPM > b.bestWPM;
    });

    cout << "\n===== Typing Race Leaderboard (Top 10) =====\n";
    int limit = min(10, (int)leaderboard.size());
    for(int i=0;i<limit;i++) {
        string color = RESET;
        if(i==0) color=GOLD;
        else if(i==1) color=SILVER;
        else if(i==2) color=BRONZE;

        double avgWPM = leaderboard[i].attempts>0 ? leaderboard[i].totalWPM/leaderboard[i].attempts : 0;

        cout << color << i+1 << ". " << leaderboard[i].username
             << " | Best WPM: " << leaderboard[i].bestWPM
             << " | Avg WPM: " << avgWPM
             << " | Accuracy: " << leaderboard[i].bestAccuracy
             << "% | Attempts: " << leaderboard[i].attempts << RESET << endl;
    }
    cout << "===========================================\n";
    pressToContinue();
}

// Typing Test
double typingTest(User &user) {
    vector<string> sentences = {
        "The quick brown fox jumps over the lazy dog",
        "Typing speed is measured in words per minute",
        "Practice makes a person perfect in typing",
        "C plus plus is a powerful programming language",
        "The weather today is bright and sunny",
        "A journey of a thousand miles begins with a step",
        "Hard work and dedication always bring success",
        "Technology is changing the world rapidly",
        "Education is the key to a better future",
        "Patience and persistence are the keys to success",
        "Reading books improves your knowledge and wisdom",
        "Learning new skills opens many opportunities in life",
        "Healthy food and exercise keep your body strong",
        "Traveling to new places broadens your perspective",
        "Listening carefully helps in better communication",
        "Time management is crucial for productivity",
        "Creativity and innovation lead to progress",
        "Teamwork and collaboration are important in work",
        "Kindness and empathy make the world better",
        "Always stay curious and keep asking questions"
    };

    srand(time(0));
    string text = sentences[rand()%sentences.size()];
    string typed;

    cout << "\n--- Previous Best ---\n";
    cout << "WPM: " << user.bestWPM << " | Accuracy: " << user.bestAccuracy << "% | Attempts: " << user.attempts << endl;

    cout << "\nType the following sentence:\n" << text << endl;

    cin.ignore();
    auto start = high_resolution_clock::now();
    getline(cin, typed);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<seconds>(stop-start).count();
    if(duration==0) duration=1;

    stringstream ss(typed);
    string word; int wordCount=0;
    while(ss >> word) wordCount++;

    double wpm = (wordCount*60.0)/duration;

    int correctChars=0;
    int len=min((int)text.size(), (int)typed.size());
    for(int i=0;i<len;i++) if(text[i]==typed[i]) correctChars++;

    double accuracy=((double)correctChars/text.size())*100.0;

    cout << "You typed " << wordCount << " words in " << duration
         << " seconds. Speed: " << wpm << " WPM, Accuracy: " << accuracy << "%\n";

    // Update records
    user.attempts++;
    user.totalWPM += wpm;

    if(wpm>user.bestWPM) {
        user.bestWPM=wpm;
        cout << GREEN << "New personal best!" << RESET << endl;
    }
    if(accuracy>user.bestAccuracy) user.bestAccuracy=accuracy;
    saveUsers();

    pressToContinue();
    return wpm;
}

// Training Mode (50 words, 10 words per round)
void trainingMode() {
    vector<string> words = {
        "apple","banana","orange","grape","cherry","school","student","teacher",
        "book","computer","keyboard","mouse","screen","internet","program","code",
        "world","country","river","mountain","forest","desert","city","village",
        "ocean","lake","bridge","road","car","bicycle","train","plane","bus",
        "flower","tree","sun","moon","star","cloud","rain","snow","wind",
        "fire","water","earth","air","stone","light","shadow"
    };

    srand(time(0));
    cout << "\n=== Typing Race Training Mode ===\n";
    cout << "Type 'exit' to quit training.\n";

    cin.ignore();
    bool continueTraining=true;
    int roundNumber=1;

    while(continueTraining){
        cout << "\n--- Round " << roundNumber << " ---\n";
        vector<string> tempWords=words;
        random_shuffle(tempWords.begin(), tempWords.end());

        int correctCount=0, wrongCount=0;

        for(int i=0;i<10;i++){
            string typed;
            auto start=high_resolution_clock::now();
            cout << "\nType this word -> " << tempWords[i] << endl;
            getline(cin, typed);
            auto stop=high_resolution_clock::now();
            auto duration=duration_cast<seconds>(stop-start).count();

            if(typed=="exit"){ continueTraining=false; break; }

            if(typed==tempWords[i]){
                cout << GREEN << "Correct!" << RESET;
                correctCount++;
            }
            else{
                cout << RED << "Wrong!" << RESET << " (Correct: " << tempWords[i] << ")";
                wrongCount++;
            }
            cout << " | Time: " << duration << "s\n";
        }

        cout << "\nRound " << roundNumber << " Complete! Correct: " << correctCount
             << ", Wrong: " << wrongCount << endl;

        if(continueTraining){
            cout << "\nDo you want to continue training? (y/n): ";
            string choice; getline(cin, choice);
            if(choice!="y" && choice!="Y") continueTraining=false;
            roundNumber++;
        }
    }

    pressToContinue();
}

int main() {
    loadUsers();

    cout << "===== Welcome to Typing Race =====\n";
    cout << "1. Register\n2. Login\nChoice: ";
    int choice; cin >> choice;

    string username, password;
    User* currentUser = nullptr;

    if(choice == 1) {
        cout << "Enter username: "; cin >> username;
        cout << "Enter password: "; cin >> password;
        if(registerUser(username, password)) {
            cout << GREEN << "Registration successful. You are now logged in!" << RESET << endl;
            currentUser = loginUser(username, password);
        } else {
            cout << RED << "Username already exists!" << RESET << endl;
            return 0;
        }
    }
    else if(choice == 2) {
        while(true) {
            cout << "Enter username: "; cin >> username;
            cout << "Enter password: "; cin >> password;
            currentUser = loginUser(username, password);

            if(currentUser) {
                cout << GREEN << "Welcome, " << currentUser->username << "!" << RESET << endl;
                break; // exit loop on success
            } else {
                cout << RED << "Invalid login! Try again (or type 'exit' as username to quit)." << RESET << endl;
                if(username == "exit") return 0;
            }
        }
    }

    while(true) {
        cout << "\n===== Menu =====\n";
        cout << "1. Normal Typing Test\n2. Training Mode\n3. Leaderboard\n4. Exit\nChoice: ";
        cin >> choice;

        if(choice == 1) typingTest(*currentUser);
        else if(choice == 2) trainingMode();
        else if(choice == 3) showLeaderboard();
        else break;
    }

    return 0;
}

