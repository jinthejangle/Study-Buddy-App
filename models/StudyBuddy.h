#ifndef STUDYBUDDY_H
#define STUDYBUDDY_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <ctime>

using namespace std;

// ------------------------- Models -------------------------
struct User {
    int id{};
    string name, email, password_hash;
    vector<string> courses;
};

struct TimeSlot {
    int id{};
    int user_id{};
    time_t start{}, end{};
    bool is_available{true};
};

struct StudySession {
    int id{};
    string course;
    time_t when{};
    string location;
    vector<int> participants;
    string status{"pending"};
};

// ------------------------- Global variables -------------------------
extern int NEXT_USER_ID, NEXT_SLOT_ID, NEXT_SESSION_ID;
extern vector<User> USERS;
extern vector<TimeSlot> SLOTS;
extern vector<StudySession> SESSIONS;

// ------------------------- Function declarations -------------------------
string trim(string s);
vector<string> split_csv(const string& line);
string hash_pw(const string& pw);
bool parse_dt(const string& s, time_t& t);
string fmt_dt(time_t t);
vector<pair<time_t,time_t>> overlaps(const vector<TimeSlot>& a, const vector<TimeSlot>& b);
User* find_user_by_email(const string& email);
User* get_user(int id);
vector<User*> users_in_course(const string& c);
vector<TimeSlot> slots_for(int uid, bool only_available=false);
vector<StudySession> sessions_for(int uid);
string ask(const string& prompt);
int ask_int(const string& prompt);
void view_profile(User& u);
void manage_availability(User& u);
void schedule_session(User& a, User& b, const string& course,
                     const vector<pair<time_t,time_t>>& commons);
void find_study_buddy(User& me);
void view_sessions(User& u);
User* do_login();
void do_register();
void dashboard(User& me);

#endif