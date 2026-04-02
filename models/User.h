#pragma once
#include <string>
#include <iostream>
using namespace std;

/*
 * Abstract Base Class: User
 * Demonstrates: Abstraction (pure virtual functions), Encapsulation,
 *               Virtual Destructor, Polymorphism
 */
class User {
protected:
    string username;
    string password;
    string role;   // "admin" or "passenger"

public:
    User() = default;
    User(const string& u, const string& p, const string& r)
        : username(u), password(p), role(r) {}

    // Pure virtual functions — must be overridden by derived classes
    virtual void login() = 0;
    virtual void displayMenu() = 0;

    // Virtual destructor — essential for proper cleanup through base pointers
    virtual ~User() {}

    // Encapsulation: getters and setters
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRole() const { return role; }
    void setUsername(const string& u) { username = u; }
    void setPassword(const string& p) { password = p; }
    void setRole(const string& r) { role = r; }
};
