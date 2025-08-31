#include "../models/StudyBuddy.h"

// Add some sample data for testing
    USERS.push_back({NEXT_USER_ID++, "Alice Johnson", "alice@clemson.edu", hash_pw("password123"), {"CPSC1010", "MATH2060"}});
    USERS.push_back({NEXT_USER_ID++, "Bob Smith", "bob@clemson.edu", hash_pw("password123"), {"CPSC1010", "PHYS2070"}});

bool test_find_user_by_email_success() {
    User* test_user;
    test_user = find_user_by_email("alice@clemson.edu");
    if (test_user->name == "Alice Johnson") {
        return true;
    }
    else {
        return false;
    }
}

bool test_find_user_by_email_fail() {
    User* test_user2;
    test_user2 = find_user_by_email("N/A");
    if (test_user2 == nullptr) {
        return true;
    }
    else {
        return false;
    }
}

bool test_get_user_success() {
    User* test_user3;
    test_user3 = get_user(2);
    if (test_user3->name == "Bob Smith") {
        return true;
    }
    else {
        return false;
    }
}

bool test_get_user_fail() {
    User* test_user4;
    test_user4 = get_user(4);
    if (test_user4 == nullptr) {
        return true;
    }
    else {
        return false;
    }
}

bool test_users_in_course() {
    vector<User*> test_list;
    test_list = users_in_course("CPSC1010");
    if (test_list = USERS) {
        return true;
    }
    else {
        return false;
    }
}

//Test One
cout << "\nTest One: ";
if (test_find_user_by_email_success() == true) {
    cout << "Pass!";
}
else {
    cout << "Fail.";
}

//Test Two
cout << "\nTest Two: ";
if (test_find_user_by_email_fail() == true) {
    cout << "Pass!";
}
else {
    cout << "Fail.";
}

//Test Three
cout << "\nTest Three: ";
if (test_get_user_success() == true) {
    cout << "Pass!";
}
else {
    cout << "Fail.";
}

//Test Four
cout << "\nTest Four: ";
if (test_get_user_fail == true) {
    cout << "Pass!";
}
else {
    cout << "Fail.";
}

//Test Five
cout << "\nTest Five: ";
if (test_users_in_course() == true) {
    cout << "Pass!";
}
else {
    cout << "Fail.";
}