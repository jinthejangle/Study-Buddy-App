#include "StudyBuddy.h"

// ------------------------- Main -------------------------
int main(){
    // Add some sample data for testing
    USERS.push_back({NEXT_USER_ID++, "Alice Johnson", "alice@clemson.edu", hash_pw("password123"), {"CPSC1010", "MATH2060"}});
    USERS.push_back({NEXT_USER_ID++, "Bob Smith", "bob@clemson.edu", hash_pw("password123"), {"CPSC1010", "PHYS2070"}});
    
    while (true){
        cout << "Welcome to Study Buddy\n1) Login\n2) Register\n3) Exit\n";
        string c = ask("Choice: ");
        if (c=="1"){ 
            if (auto* u = do_login()) dashboard(*u); 
        }
        else if (c=="2") do_register();
        else if (c=="3") { cout<<"Goodbye!\n"; break; }
        else cout<<"?\n";
    }
    return 0;
}