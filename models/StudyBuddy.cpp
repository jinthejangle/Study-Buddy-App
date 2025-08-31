#include "StudyBuddy.h"

// ------------------------- Global variable definitions -------------------------
int NEXT_USER_ID = 1, NEXT_SLOT_ID = 1, NEXT_SESSION_ID = 1;
vector<User> USERS;
vector<TimeSlot> SLOTS;
vector<StudySession> SESSIONS;

// ------------------------- Utility implementations -------------------------
string trim(string s){
    auto ws = [](int c){ return isspace(static_cast<unsigned char>(c)); };
    while(!s.empty() && ws(s.front())) s.erase(s.begin());
    while(!s.empty() && ws(s.back()))  s.pop_back();
    return s;
}

vector<string> split_csv(const string& line){
    vector<string> out; string item; stringstream ss(line);
    while(getline(ss,item,',')){ item = trim(item); if(!item.empty()) out.push_back(item); }
    sort(out.begin(), out.end()); out.erase(unique(out.begin(), out.end()), out.end());
    return out;
}

string hash_pw(const string& pw){ return "h#" + pw; }

bool parse_dt(const string& s, time_t& t){
    if (s.size()!=16 || s[4]!='-'||s[7]!='-'||s[10]!=' '||s[13]!=':') return false;
    tm tmv{}; tmv.tm_isdst = -1;
    try{
        tmv.tm_year = stoi(s.substr(0,4)) - 1900;
        tmv.tm_mon  = stoi(s.substr(5,2)) - 1;
        tmv.tm_mday = stoi(s.substr(8,2));
        tmv.tm_hour = stoi(s.substr(11,2));
        tmv.tm_min  = stoi(s.substr(14,2));
        tmv.tm_sec  = 0;
    }catch(...){ return false; }
    t = mktime(&tmv);
    return t != time_t(-1);
}

string fmt_dt(time_t t){
    tm tmv{}; 
    #ifdef _WIN32
    localtime_s(&tmv, &t);
    #else
    localtime_r(&t, &tmv);
    #endif
    char buf[32]; 
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tmv);
    return string(buf);
}

vector<pair<time_t,time_t>> overlaps(const vector<TimeSlot>& a, const vector<TimeSlot>& b){
    vector<pair<time_t,time_t>> out;
    for (auto& x : a) if (x.is_available)
    for (auto& y : b) if (y.is_available){
        if (x.start < y.end && x.end > y.start){
            out.push_back({ max(x.start,y.start), min(x.end,y.end) });
        }
    }
    sort(out.begin(), out.end());
    out.erase(unique(out.begin(), out.end()), out.end());
    return out;
}

// ------------------------- Database helpers -------------------------
User* find_user_by_email(const string& email){
    for (auto& u: USERS) if (u.email==email) return &u; return nullptr;
}

User* get_user(int id){
    for (auto& u: USERS) if (u.id==id) return &u; return nullptr;
}

vector<User*> users_in_course(const string& c){
    vector<User*> v;
    for (auto& u: USERS)
        if (find(u.courses.begin(), u.courses.end(), c)!=u.courses.end()) v.push_back(&u);
    return v;
}

vector<TimeSlot> slots_for(int uid, bool only_available){
    vector<TimeSlot> v;
    for (auto& s: SLOTS) if (s.user_id==uid && (!only_available || s.is_available)) v.push_back(s);
    sort(v.begin(), v.end(), [](auto&a, auto&b){return a.start<b.start;});
    return v;
}

vector<StudySession> sessions_for(int uid){
    vector<StudySession> v;
    for (auto& s: SESSIONS)
        if (find(s.participants.begin(), s.participants.end(), uid)!=s.participants.end()
            && s.status!="cancelled") v.push_back(s);
    sort(v.begin(), v.end(), [](auto&a, auto&b){return a.when<b.when;});
    return v;
}

// ------------------------- I/O helpers -------------------------
string ask(const string& prompt){
    cout << prompt << flush;
    string s; getline(cin, s);
    if (!s.empty() && s[0] == ' ') s = s.substr(1);
    if (!s.empty() && s.back() == ' ') s.pop_back();
    return s;
}

int ask_int(const string& prompt){
    while (true){
        string s = ask(prompt);
        try{ return stoi(s); }catch(...){ cout<<"Please enter a number.\n"; }
    }
}

// ------------------------- Flow implementations -------------------------
void view_profile(User& u){
    cout << "\n--- My Profile ---\n";
    cout << "Name: " << u.name << "\nEmail: " << u.email << "\nCourses: ";
    if (u.courses.empty()) cout << "(none)\n"; else {
        for (size_t i=0;i<u.courses.size();++i){ if(i) cout<<", "; cout<<u.courses[i]; } cout<<'\n';
    }
    cout << "1) Add course  2) Remove course  3) Back\n";
    string c = ask("Choice: ");
    if (c=="1"){
        string nc = trim(ask("Enter course to add: "));
        if (!nc.empty() && find(u.courses.begin(), u.courses.end(), nc)==u.courses.end()){
            u.courses.push_back(nc); sort(u.courses.begin(), u.courses.end()); cout<<"Added.\n";
        } else cout<<"Already there or invalid.\n";
    } else if (c=="2"){
        if (u.courses.empty()) { cout<<"None.\n"; return; }
        for (size_t i=0;i<u.courses.size();++i) cout<<i+1<<") "<<u.courses[i]<<"\n";
        int k = ask_int("Pick #: "); if (k>=1 && k<=(int)u.courses.size()){
            u.courses.erase(u.courses.begin()+k-1); cout<<"Removed.\n";
        }
    }
}

void manage_availability(User& u){
    while (true){
        cout << "\n--- My Availability ---\n";
        auto mine = slots_for(u.id);
        if (mine.empty()) cout<<"(none)\n";
        for (auto& s: mine)
            cout << "#" << s.id << "  " << fmt_dt(s.start) << " — " << fmt_dt(s.end)
                 << (s.is_available? " [available]":" [off]") << '\n';

        cout << "1) Add slot  2) Remove slot  3) Back\n";
        string c = ask("Choice: ");
        if (c=="1"){
            time_t a,b; string sa = ask("Start (YYYY-MM-DD HH:MM): ");
            string sb = ask("End   (YYYY-MM-DD HH:MM): ");
            if (!parse_dt(sa,a) || !parse_dt(sb,b) || !(a<b)){ cout<<"Invalid.\n"; continue; }
            SLOTS.push_back({NEXT_SLOT_ID++, u.id, a, b, true});
            cout<<"Added.\n";
        } else if (c=="2"){
            int id = ask_int("Slot id: ");
            auto it = find_if(SLOTS.begin(), SLOTS.end(), [&](const TimeSlot& s){return s.id==id && s.user_id==u.id;});
            if (it!=SLOTS.end()){ SLOTS.erase(it); cout<<"Removed.\n"; } else cout<<"Not found.\n";
        } else if (c=="3") break; else cout<<"?\n";
    }
}

void schedule_session(User& a, User& b, const string& course,
                     const vector<pair<time_t,time_t>>& commons){
    cout << "\nScheduling with " << b.name << "\nPick a time:\n";
    for (size_t i=0;i<commons.size();++i)
        cout << i+1 << ") " << fmt_dt(commons[i].first) << " — " << fmt_dt(commons[i].second) << '\n';
    int k = ask_int("Choice #: "); if (k<1 || k>(int)commons.size()) return;
    string loc = ask("Location (e.g., Cooper Library 2nd floor / Zoom): ");
    StudySession s;
    s.id = NEXT_SESSION_ID++; s.course = course; s.when = commons[k-1].first;
    s.location = loc; s.participants = {a.id, b.id}; s.status = "pending";
    SESSIONS.push_back(s);
    cout << "Invite sent to " << b.name << ". Scheduled for " << fmt_dt(s.when)
         << " at " << s.location << ".\n";
}

void find_study_buddy(User& me){
    if (me.courses.empty()){ cout<<"Add a course first.\n"; return; }
    cout << "\n--- Find a Study Buddy ---\n";
    for (size_t i=0;i<me.courses.size();++i) cout<<i+1<<") "<<me.courses[i]<<"\n";
    int pick = ask_int("Course #: "); if (pick<1 || pick>(int)me.courses.size()) return;
    string course = me.courses[pick-1];

    auto my = slots_for(me.id, true);
    auto classmates = users_in_course(course);
    classmates.erase(remove(classmates.begin(), classmates.end(), &me), classmates.end());

    if (classmates.empty()){ cout<<"No other students found for "<<course<<".\n"; return; }

    struct Match { User* who; vector<pair<time_t,time_t>> commons; };
    vector<Match> ms;

    for (auto* u : classmates){
        auto theirs = slots_for(u->id, true);
        auto common  = overlaps(my, theirs);
        if (!common.empty()) ms.push_back({u, common});
    }

    if (ms.empty()){ cout<<"No available matches found for "<<course<<".\n"; return; }
    cout << "Suggested Study Buddies for " << course << ":\n";
    for (size_t i=0;i<ms.size();++i)
        cout << i+1 << ") " << ms[i].who->name << "  [" << ms[i].commons.size() << " matching slots]\n";

    int k = ask_int("Buddy #: (0 to back) ");
    if (k>=1 && k<=(int)ms.size()) schedule_session(me, *ms[k-1].who, course, ms[k-1].commons);
}

void view_sessions(User& u){
    cout << "\n--- My Upcoming Study Sessions ---\n";
    auto v = sessions_for(u.id);
    if (v.empty()){ cout<<"No upcoming sessions.\n"; return; }
    for (auto& s : v){
        vector<string> others;
        for (int uid : s.participants) if (uid!=u.id) if (auto* p=get_user(uid)) others.push_back(p->name);
        cout << "Course: " << s.course << "\nTime: " << fmt_dt(s.when)
             << "\nWith: " << (others.empty()? "(n/a)" : others.front())
             << "\nLocation: " << s.location << "\nStatus: " << s.status << "\n-----\n";
    }
}

User* do_login(){
    string email = ask("Email: ");
    string pw    = ask("Password: ");
    if (auto* u = find_user_by_email(email)){
        if (u->password_hash == hash_pw(pw)){ cout<<"Login successful!\n"; return u; }
    }
    cout<<"Invalid credentials.\n"; return nullptr;
}

void do_register(){
    string name = ask("Name: ");
    string email= ask("Email: ");
    if (find_user_by_email(email)){ cout<<"Email already in use.\n"; return; }
    string pw   = ask("Password: ");
    auto courses= split_csv(ask("Courses (comma-separated): "));
    USERS.push_back({NEXT_USER_ID++, name, email, hash_pw(pw), courses});
    cout<<"Registration successful! Please login.\n";
}

void dashboard(User& me){
    while (true){
        cout << "\n--- Study Buddy Dashboard ---\n"
             << "1) Manage Availability\n2) Find a Study Buddy\n3) View My Sessions\n"
             << "4) View/Edit Profile\n5) Logout\n";
        string c = ask("Choice: ");
        if (c=="1") manage_availability(me);
        else if (c=="2") find_study_buddy(me);
        else if (c=="3") view_sessions(me);
        else if (c=="4") view_profile(me);
        else if (c=="5") { cout<<"Logging out...\n"; break; }
        else cout<<"?\n";
    }
}