#ifndef COMPUTER_CLUB_STRUCTS_H
#define COMPUTER_CLUB_STRUCTS_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <regex>

struct Time {
    int hour;
    int minute;

    Time(int h, int m) : hour(h), minute(m) {}

    bool operator<(const Time &other) const {
        return hour < other.hour || (hour == other.hour && minute < other.minute);
    }

    bool operator>(const Time &other) const {
        return hour > other.hour || (hour == other.hour && minute > other.minute);
    }

    bool operator==(const Time &other) const {
        return hour == other.hour && minute == other.minute;
    }

    bool operator<=(const Time &other) const {
        return *this < other || *this == other;
    }

    bool operator>=(const Time &other) const {
        return *this > other || *this == other;
    }

    Time operator+(const Time &other) const {
        int total_minutes = (hour * 60 + minute) + (other.hour * 60 + other.minute);
        int new_hour = total_minutes / 60;
        int new_minute = total_minutes % 60;
        return {new_hour, new_minute};
    }

    // Subtraction operation
    Time operator-(const Time &other) const {
        int total_minutes = (hour * 60 + minute) - (other.hour * 60 + other.minute);
        if (total_minutes < 0) {
            total_minutes = 0;
        }
        int new_hour = total_minutes / 60;
        int new_minute = total_minutes % 60;
        return {new_hour, new_minute};
    }

    Time operator+=(const Time &other) {
        *this = *this + other;
        return *this;
    }

    // print time in HH:MM format
    friend std::ostream &operator<<(std::ostream &os, const Time &t) {
        os << (t.hour < 10 ? "0" : "") << t.hour << ":" << (t.minute < 10 ? "0" : "") << t.minute;
        return os;
    }
};

struct Client {
    std::string name;
    Time arrival_time;
    bool seated;
    int table_number;

    Client() : name(""), arrival_time(0, 0), seated(false), table_number(-1) {}

    Client(std::string n, Time t) : name(n), arrival_time(t), seated(false), table_number(-1) {}
};

struct Table {
    int number;
    int revenue;
    bool occupied;
    Time occupied_time_start;
    Time occupied_time_end;
    Time total_occupied_time;

    Table(int num) : number(num), revenue(0), occupied(false), occupied_time_start(0, 0), occupied_time_end(0, 0),
                     total_occupied_time(0, 0) {}

    // operator overloading for printing table info
    friend std::ostream &operator<<(std::ostream &os, const Table &t) {
        os << t.number << " " << t.revenue << " " << t.total_occupied_time;
        return os;
    }
};

struct Event {
    Time time;
    int ID;
    std::string body;

    Event(Time t, int id, std::string b) : time(t), ID(id), body(b) {}

    friend std::ostream &operator<<(std::ostream &os, const Event &e) {
        // '12:48 1 client1 14' format
        os << e.time << " " << e.ID << " " << e.body;
        return os;
    }
};

#endif // COMPUTER_CLUB_STRUCTS_H
