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
            // Handle negative time difference
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

// TODO: split this function into smaller functions
void parse_input(const std::string &filename, int &num_of_tables, Time &start_time, Time &end_time, int &cost_per_hour,
                 std::vector<Event> &events) {
    try {
        std::ifstream input_file(filename);
        if (!input_file.is_open()) {
            throw std::runtime_error("Error: cannot open input file <" + filename + ">");
        }

        std::string line;
        std::getline(input_file, line);
        num_of_tables = std::stoi(line);

        std::getline(input_file, line); // 09:00 21:00
        std::string start_time_str = line.substr(0, 5);
        std::string end_time_str = line.substr(6, 5);
        std::regex time_regex("^([01]?[0-9]|2[0-3]):[0-5][0-9]$");
        bool valid_start_time = std::regex_match(start_time_str, time_regex);
        bool valid_end_time = std::regex_match(end_time_str, time_regex);
        if (!valid_start_time || !valid_end_time) {
            throw std::runtime_error("Invalid time format. Time should be in HH:MM format with leading zeros.");
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
        cost_per_hour = std::stoi(line);

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
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

std::optional<Event> handle_client_arrival(const Time &arrival_time, const std::string &event_body,
                                           std::unordered_map<std::string, Client> &clients,
                                           std::queue<Client> &waiting_list,
                                           const Time &start_time, const Time &end_time) {
    std::string client_name = event_body;
    bool valid_name = std::regex_match(client_name, std::regex("^[a-zA-Z0-9_-]+$"));
    if (!valid_name) {
        return Event(arrival_time, 13, "Invalid client name: " + client_name);
    }

    bool client_already_exists = clients.find(client_name) != clients.end();
    if (client_already_exists) {
        return Event(arrival_time, 13, "YouShallNotPass");
    }

    bool valid_arrival_time = arrival_time >= start_time && arrival_time <= end_time;
    if (!valid_arrival_time) {
        return Event(arrival_time, 13, "NotOpenYet");
    }

    Client new_client(client_name, arrival_time);
    clients[client_name] = new_client;

    return std::nullopt;
}

std::optional<Event> handle_client_sit(const Time &event_time, const std::string &event_body,
                                       std::unordered_map<std::string, Client> &clients, std::vector<Table> &tables,
                                       std::queue<Client> &waiting_list, const Time &start_time, const Time &end_time) {
    std::regex sit_regex("^([a-z0-9_-]+) (\\d+)$");
    std::smatch match;
    if (!std::regex_match(event_body, match, sit_regex)) {
        return Event(event_time, 13, "Error: invalid sit event body: <" + event_body + ">");
    }

    std::string client_name = match[1];
    int table_number = std::stoi(match[2]);

    if (table_number < 1 || table_number > tables.size()) {
        return Event(event_time, 13, "Error: table number <" + std::to_string(table_number) + "> is out of range");
    }

    --table_number; // tables are 0-indexed
    bool desired_table_is_occupied = tables[table_number].occupied;
    if (desired_table_is_occupied) {
        return Event(event_time, 13, "PlaceIsBusy");
    }

    bool client_exists = clients.find(client_name) != clients.end();
    if (!client_exists) {
        return Event(event_time, 13, "ClientUnknown");
    }

    tables[table_number].occupied = true;
    tables[table_number].occupied_time_start = event_time;
    clients[client_name].table_number = table_number;
    clients[client_name].seated = true;

    return std::nullopt;
}
std::optional<Event> handle_client_start_waiting(const Time &event_time, const std::string &event_body,
                                                 std::unordered_map<std::string, Client> &clients, std::vector<Table> &tables,
                                                 std::queue<Client> &waiting_list,
                                                 const Time &start_time, const Time &end_time) {
    std::string client_name = event_body;
    bool valid_name = std::regex_match(client_name, std::regex("^[a-zA-Z0-9_-]+$"));
    if (!valid_name) {
        return Event(event_time, 13, "Invalid client name: " + client_name);
    }

    bool there_is_a_table = false;
    for (const auto &table: tables) {
        if (!table.occupied) {
            there_is_a_table = true;
            break;
        }
    }

    if (there_is_a_table) {
        return Event(event_time, 13, "ICanWaitNoLonger!");
    }

    bool queue_at_full_capacity = waiting_list.size() == tables.size();
    if (queue_at_full_capacity) {
        // client leaves
        return Event(event_time, 11, client_name);
    }

    waiting_list.push(clients[client_name]);

    return std::nullopt;
}
std::optional<Event> handle_client_leave_table(const Time &event_time, const std::string &event_body,
                                         std::unordered_map<std::string, Client> &clients, std::vector<Table> &tables,
                                         std::queue<Client> &waiting_list,
                                         const Time &start_time, const Time &end_time, int cost_per_hour) {
    std::string client_name = event_body;
    bool valid_name = std::regex_match(client_name, std::regex("^[a-zA-Z0-9_-]+$"));
    if (!valid_name) {
        return Event(event_time, 13, "Invalid client name: " + client_name);
    }

    bool client_exists = clients.find(client_name) != clients.end();
    if (!client_exists) {
        return Event(event_time, 13, "ClientUnknown");
    }

    Client &client = clients[client_name];
    if (!client.seated) {
        return Event(event_time, 13, "Error: client " + client_name + " is not seated");
    }

    int table_number = client.table_number;
    tables[table_number].occupied = false;
    tables[table_number].occupied_time_end = event_time;
    Time client_occupied_for = tables[table_number].occupied_time_end - tables[table_number].occupied_time_start;
    tables[table_number].total_occupied_time += client_occupied_for;
    int total_hours = client_occupied_for.hour + (client_occupied_for.minute > 0 ? 1 : 0);
    tables[table_number].revenue += total_hours * cost_per_hour;

    clients.erase(client_name);

    // find a client from the waiting list
    if (!waiting_list.empty()) {
        Client next_client = waiting_list.front();
        waiting_list.pop();
        return Event(event_time, 12, next_client.name + " " + std::to_string(table_number + 1));
    }

    return std::nullopt;
}
std::optional<Event> handle_client_leave(const Time &event_time, const std::string &event_body,
                                        std::unordered_map<std::string, Client> &clients, std::vector<Table> &tables,
                                        std::queue<Client> &waiting_list,
                                        const Time &start_time, const Time &end_time, int cost_per_hour) {
    std::string client_name = event_body;
    bool valid_name = std::regex_match(client_name, std::regex("^[a-zA-Z0-9_-]+$"));
    if (!valid_name) {
        return Event(event_time, 13, "Invalid client name: " + client_name);
    }

    bool client_exists = clients.find(client_name) != clients.end();
    if (!client_exists) {
        return Event(event_time, 13, "ClientUnknown");
    }

    Client &client = clients[client_name];
    if (client.seated) {
        int table_number = client.table_number;
        tables[table_number].occupied = false;
        tables[table_number].occupied_time_end = event_time;
        Time client_occupied_for = tables[table_number].occupied_time_end - tables[table_number].occupied_time_start;
        tables[table_number].total_occupied_time += client_occupied_for;
        int total_hours = client_occupied_for.hour + (client_occupied_for.minute > 0 ? 1 : 0);
        tables[table_number].revenue += total_hours * cost_per_hour;
    }

    // if client was at waiting list, remove it, keeping the order
    std::queue<Client> temp_queue;
    while (!waiting_list.empty()) {
        if (waiting_list.front().name != client_name) {
            temp_queue.push(waiting_list.front());
        }
        waiting_list.pop();
    }
    waiting_list = temp_queue;

    clients.erase(client_name);

    return std::nullopt;
}

void process_events(std::vector<Event> &events, std::unordered_map<std::string, Client> &clients,
                    std::vector<Table> &tables, std::queue<Client> &waiting_list, const Time &start_time,
                    const Time &end_time, int cost_per_hour) {
    // TODO: handle case if next event is after end_time
    size_t i = 0;
    size_t events_size = events.size();
    while (i < events_size) {
        Event &event = events[i];
        std::cout << event << std::endl;

        if (event.time > end_time) {
            Event late_event(end_time, 13, "Error: event is after closing time");
            std::cout << late_event << std::endl;
            ++i;
            continue;
        }

        std::optional<Event> new_event;

        switch (event.ID) {
            case 1:
                new_event = handle_client_arrival(event.time, event.body, clients, waiting_list, start_time, end_time);
                break;
            case 2:
                new_event = handle_client_sit(event.time, event.body, clients, tables, waiting_list, start_time,
                                              end_time);
                break;
            case 3:
                new_event = handle_client_start_waiting(event.time, event.body, clients, tables, waiting_list,
                                                        start_time, end_time);
                break;
            case 4:
                new_event = handle_client_leave_table(event.time, event.body, clients, tables, waiting_list, start_time,
                                                end_time, cost_per_hour);
                break;
            case 11:
                handle_client_leave(event.time, event.body, clients, tables, waiting_list, start_time, end_time, cost_per_hour);
                break;
            case 12:
                // this should not give a new event (table exists, client exists, too)
                handle_client_sit(event.time, event.body, clients, tables, waiting_list, start_time, end_time);
                break;
            case 13:
                // no action needed as error message is already printed
                // you can add error handling here
                break;
        }

        if (new_event.has_value()) {
            event = new_event.value();
            if (i > 0) {
                --i;
            }
            else
            {
                continue;
            }
        }

        ++i;
    }

    std::vector<std::string> clients_to_leave;
    for (const auto &client: clients) {
        clients_to_leave.push_back(client.first);
    }

    std::sort(clients_to_leave.begin(), clients_to_leave.end());

    for (const auto &client_name: clients_to_leave) {
        Event leave_event(end_time, 11, client_name);
        handle_client_leave_table(leave_event.time, leave_event.body, clients, tables, waiting_list, start_time,
                                  end_time, cost_per_hour);
    }
}

int main(int argc, char *argv[]) {
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
    for (int i = 1; i <= num_of_tables; ++i) {
        tables.push_back(Table(i));
    }

    std::cout << start_time << std::endl;
    process_events(events, clients, tables, waiting_list, start_time, end_time, cost_per_hour);
    std::cout << end_time << std::endl;

    // print tables
    for (const auto &table: tables) {
        std::cout << table << std::endl;
    }

    return 0;
}