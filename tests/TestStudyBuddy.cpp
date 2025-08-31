#include "../models/StudyBuddy.h"
#include <iostream>
#include <cassert>

using namespace std;

void initializeTestData() {
    // Clear any existing data
    USERS.clear();
    SLOTS.clear();
    SESSIONS.clear();
    NEXT_USER_ID = 1;
    NEXT_SLOT_ID = 1;
    NEXT_SESSION_ID = 1;

    // Add sample data for testing
    USERS.push_back({NEXT_USER_ID++, "Alice Johnson", "alice@clemson.edu", hash_pw("password123"), {"CPSC1010", "MATH2060"}});
    USERS.push_back({NEXT_USER_ID++, "Bob Smith", "bob@clemson.edu", hash_pw("password123"), {"CPSC1010", "PHYS2070"}});
    USERS.push_back({NEXT_USER_ID++, "Carol Williams", "carol@clemson.edu", hash_pw("password123"), {"MATH2060", "ENGL1030"}});
}

bool test_find_user_by_email_success() {
    User* test_user = find_user_by_email("alice@clemson.edu");
    return test_user != nullptr && test_user->name == "Alice Johnson";
}

bool test_find_user_by_email_fail() {
    User* test_user = find_user_by_email("nonexistent@email.com");
    return test_user == nullptr;
}

bool test_get_user_success() {
    User* test_user = get_user(2);
    return test_user != nullptr && test_user->name == "Bob Smith";
}

bool test_get_user_fail() {
    User* test_user = get_user(999);
    return test_user == nullptr;
}

bool test_users_in_course() {
    vector<User*> test_list = users_in_course("CPSC1010");
    return test_list.size() == 2; // Alice and Bob should be in CPSC1010
}

bool test_users_in_course_none() {
    vector<User*> test_list = users_in_course("NONEXISTENT");
    return test_list.empty();
}

bool test_slots_for_user() {
    // Add a test time slot
    time_t now = time(nullptr);
    time_t later = now + 3600; // 1 hour later
    SLOTS.push_back({NEXT_SLOT_ID++, 1, now, later, true});
    
    vector<TimeSlot> slots = slots_for(1);
    return !slots.empty();
}

bool test_hash_password() {
    string hashed = hash_pw("test123");
    return hashed == "h#test123";
}

void runAllTests() {
    initializeTestData();
    
    cout << "=== Running Study Buddy Tests ===\n";
    
    // Test 1
    cout << "Test 1 - Find user by email (success): ";
    cout << (test_find_user_by_email_success() ? "PASS!" : "FAIL!") << endl;
    
    // Test 2
    cout << "Test 2 - Find user by email (fail): ";
    cout << (test_find_user_by_email_fail() ? "PASS!" : "FAIL!") << endl;
    
    // Test 3
    cout << "Test 3 - Get user by ID (success): ";
    cout << (test_get_user_success() ? "PASS!" : "FAIL!") << endl;
    
    // Test 4
    cout << "Test 4 - Get user by ID (fail): ";
    cout << (test_get_user_fail() ? "PASS!" : "FAIL!") << endl;
    
    // Test 5
    cout << "Test 5 - Users in course: ";
    cout << (test_users_in_course() ? "PASS!" : "FAIL!") << endl;
    
    // Test 6
    cout << "Test 6 - Users in non-existent course: ";
    cout << (test_users_in_course_none() ? "PASS!" : "FAIL!") << endl;
    
    // Test 7
    cout << "Test 7 - Slots for user: ";
    cout << (test_slots_for_user() ? "PASS!" : "FAIL!") << endl;
    
    // Test 8
    cout << "Test 8 - Password hashing: ";
    cout << (test_hash_password() ? "PASS!" : "FAIL!") << endl;
    
    cout << "=== Tests Complete ===\n";
}

int main() {
    runAllTests();
    return 0;
}