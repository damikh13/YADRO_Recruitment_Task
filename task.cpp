#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

struct Time {
    int hour;
    int minute;

    Time(int h, int m) : hour(h), minute(m) {}

    bool operator<(const Time& other) const {
        return hour < other.hour || (hour == other.hour && minute < other.minute);
    }

    bool operator>(const Time& other) const {
        return hour > other.hour || (hour == other.hour && minute > other.minute);
    }

    bool operator==(const Time& other) const {
        return hour == other.hour && minute == other.minute;
    }

    bool operator<=(const Time& other) const {
        return *this < other || *this == other;
    }

    bool operator>=(const Time& other) const {
        return *this > other || *this == other;
    }

    // print time in HH:MM format
    friend std::ostream& operator<<(std::ostream& os, const Time& t) {
        os << (t.hour < 10 ? "0" : "") << t.hour << ":" << (t.minute < 10 ? "0" : "") << t.minute;
        return os;
    }
};
struct Client {
    std::string name;
    Time arrival_time;
    bool is_seated;
    int table_number;

    Client(std::string n, Time t) : name(n), arrival_time(t), is_seated(false), table_number(-1) {}
};
struct Table {
    int number;
    int revenue;
    Time occupiedTime;

    Table(int num) : number(num), revenue(0), occupiedTime(0, 0) {}
};
struct Event {
    Time time;
    int ID;
    std::string body;

    Event(Time t, int id, std::string b) : time(t), ID(id), body(b) {}
};

// TODO: split this function into smaller functions
void parse_input(const std::string& filename, int& num_of_tables, Time& start_time, Time& end_time, int& cost_per_hour, std::vector<Event>& events)
{
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cout << "Error: cannot open input file <" << filename << ">" << std::endl;
        return;
    }

    std::string line;
    std::getline(input_file, line);
    try {
        num_of_tables = std::stoi(line);
    } catch (std::invalid_argument& e) {
        std::cout << line << std::endl;
        return;
    }

    std::getline(input_file, line); // 09:00 21:00
    std::string start_time_str = line.substr(0, 5);
    std::string end_time_str = line.substr(6, 5);
    std::regex time_regex("^([01]?[0-9]|2[0-3]):[0-5][0-9]$");
    bool valid_start_time = std::regex_match(start_time_str, time_regex);
    bool valid_end_time = std::regex_match(end_time_str, time_regex);
    if (!valid_start_time || !valid_end_time) {
        std::cout << "Invalid time format. Time should be in HH:MM format with leading zeros." << std::endl;
        return;
    }
    // parse start time
    std::string hour_str = start_time_str.substr(0, 2);
    std::string minute_str = start_time_str.substr(3, 2);
    int hour = std::stoi(hour_str);
    int minute = std::stoi(minute_str);
    start_time = Time(hour, minute);

    // parse end time
    hour_str = end_time_str.substr(0, 2);
    minute_str = end_time_str.substr(3, 2);
    hour = std::stoi(hour_str);
    minute = std::stoi(minute_str);
    end_time = Time(hour, minute);

    std::getline(input_file, line);
    try {
        cost_per_hour = std::stoi(line);
    } catch (std::invalid_argument& e) {
        std::cout << line << std::endl;
        return;
    }

    std::regex eventRegex(R"((\d{2}:\d{2}) (\d+) (.+))"); // 09:00 4 John or 18:00 12 Mary 4
    std::smatch match;

    while (std::getline(input_file, line)) {
        if (std::regex_match(line, match, eventRegex)) {
            std::string time_str = match[1];
            std::string ID = match[2];
            std::string body = match[3];

            std::string hour_str = time_str.substr(0, 2);
            std::string minute_str = time_str.substr(3, 2);
            int hour = std::stoi(hour_str);
            int minute = std::stoi(minute_str);
            Time time(hour, minute);

            int ID_int = std::stoi(ID);

            events.push_back(Event(time, ID_int, body));
        }
    }

    input_file.close();
}

void handle_client_arrival(const Time& arrival_time, const std::string& event_body, std::unordered_map <std::string, Client>& clients, std::queue<Client>& waiting_list, const Time& start_time, const Time& end_time)
{
    std::string client_name = event_body;
    bool valid_name = std::regex_match(client_name, std::regex("^[a-zA-Z0-9_-]+$"));
    if (!valid_name) {
        std::cout << "Invalid client name: " << client_name << std::endl;
        return;
    }

    bool client_already_exists = clients.find(client_name) != clients.end();
    if (client_already_exists) {
        std::cout << "YouShallNotPass" << std::endl;
        return;
    }

    bool valid_arrival_time = arrival_time >= start_time && arrival_time <= end_time;
    if (!valid_arrival_time) {
        std::cout << "NotOpenYet" << std::endl;
        return;
    }

    Client new_client(client_name, arrival_time);
    clients[client_name] = new_client;
    waiting_list.push(new_client);
}
void process_events(const std::vector<Event>& events, std::unordered_map<std::string, Client>& clients, std::vector<Table>& tables, std::queue<Client>& waiting_list)
{
    // TODO: optimize adding new events
    for (size_t i = 0; i < events.size(); ++i) {
        const Event& event = events[i];
        switch (event.ID) {
            case 1:
                // arrival
                break;
            case 2:
                // sits at table
                break;
            case 3:
                // waits for table to be free
                break;
            case 4:
                // leaves
                break;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int num_of_tables;
    Time start_time(0, 0);
    Time end_time(0, 0);
    int cost_per_hour;
    std::vector<Event> events;
    std::unordered_map<std::string, Client> clients;
    std::vector<Table> tables;
    parse_input(filename, num_of_tables, start_time, end_time, cost_per_hour, events);
    std::queue<Client> waiting_list;

    tables.reserve(num_of_tables); // performance boost if N is big
    for (int i = 1; i <= num_of_tables; i++) {
        tables.push_back(Table(i));
    }

    return 0;
}