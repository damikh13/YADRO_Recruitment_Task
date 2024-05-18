#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include "Computer_Club_STRUCTS.h"

// PARSING FUNCTIONS
std::ifstream open_input_file(const std::string &filename) {
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        throw std::runtime_error("Error: cannot open input file <" + filename + ">");
    }
    return input_file;
}

int parse_num_of_tables(std::ifstream &input_file) {
    std::string line;
    std::getline(input_file, line);
    return std::stoi(line);
}

Time parse_time(const std::string &time_str) {
    std::regex time_regex("^([01]?[0-9]|2[0-3]):[0-5][0-9]$");
    if (!std::regex_match(time_str, time_regex)) {
        throw std::runtime_error("Invalid time format. Time should be in HH:MM format with leading zeros.");
    }
    std::string hour_str = time_str.substr(0, 2);
    std::string minute_str = time_str.substr(3, 2);
    int hour = std::stoi(hour_str);
    int minute = std::stoi(minute_str);
    return {hour, minute};
}

int parse_cost_per_hour(std::ifstream &input_file) {
    std::string line;
    std::getline(input_file, line);
    return std::stoi(line);
}

std::vector<Event> parse_events(std::ifstream &input_file) {
    std::vector<Event> events;
    std::regex event_regex(R"((\d{2}:\d{2}) (\d+) (.+))"); // 09:00 4 John or 18:00 12 Mary 4
    std::smatch match;
    std::string line;
    while (std::getline(input_file, line)) {
        bool valid_event = std::regex_match(line, match, event_regex);
        if (valid_event) {
            std::string time_str = match[1];
            std::string ID = match[2];
            std::string body = match[3];
            Time time = parse_time(time_str);
            int ID_int = std::stoi(ID);
            events.emplace_back(time, ID_int, body);
        }
    }
    return events;
}

void parse_input(const std::string &filename, int &num_of_tables, Time &start_time, Time &end_time, int &cost_per_hour,
                 std::vector<Event> &events) {
    try {
        std::ifstream input_file = open_input_file(filename);
        num_of_tables = parse_num_of_tables(input_file);

        std::string line;
        std::getline(input_file, line); // 09:00 21:00
        start_time = parse_time(line.substr(0, 5));
        end_time = parse_time(line.substr(6, 5));

        cost_per_hour = parse_cost_per_hour(input_file);
        events = parse_events(input_file);
        input_file.close();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

// EVENT HANDLING HELPER FUNCTIONS
bool is_valid_client_name(const std::string &client_name) {
    return std::regex_match(client_name, std::regex("^[a-zA-Z0-9_-]+$"));
}

bool client_exists(const std::unordered_map<std::string, Client> &clients, const std::string &client_name) {
    return clients.find(client_name) != clients.end();
}

bool is_valid_table_number(int table_number, const std::vector<Table> &tables) {
    return table_number >= 1 && table_number <= tables.size();
}

bool is_table_occupied(const std::vector<Table> &tables, int table_number) {
    return tables[table_number - 1].occupied;
}

bool is_table_available(const std::vector<Table> &tables) {
    for (const auto &table: tables) {
        if (!table.occupied) {
            return true;
        }
    }
    return false;
}

void free_table(Table &table, const Time &event_time, int cost_per_hour) {
    table.occupied = false;
    table.occupied_time_end = event_time;
    Time client_occupied_for = table.occupied_time_end - table.occupied_time_start;
    table.total_occupied_time += client_occupied_for;
    int total_hours = client_occupied_for.hour + (client_occupied_for.minute > 0 ? 1 : 0);
    table.revenue += total_hours * cost_per_hour;
}

std::queue<Client> remove_client_from_queue(std::queue<Client> &waiting_list, const std::string &client_name) {
    std::queue<Client> temp_queue;
    while (!waiting_list.empty()) {
        if (waiting_list.front().name != client_name) {
            temp_queue.push(waiting_list.front());
        }
        waiting_list.pop();
    }
    return temp_queue;
}


class Computer_Club {
private:
    std::unordered_map<std::string, Client> clients_;
    std::vector<Table> tables_;
    std::queue<Client> waiting_list_;
    Time start_time_;
    Time end_time_;
    int cost_per_hour_;

    std::optional<Event> handle_client_arrival(const Time &arrival_time, const std::string &event_body) {
        const std::string &client_name = event_body;
        if (!is_valid_client_name(client_name)) {
            return Event(arrival_time, 13, "Invalid client name: " + client_name);
        }

        if (client_exists(clients_, client_name)) {
            return Event(arrival_time, 13, "YouShallNotPass");
        }

        bool valid_arrival_time = arrival_time >= start_time_ && arrival_time <= end_time_;
        if (!valid_arrival_time) {
            return Event(arrival_time, 13, "NotOpenYet");
        }

        Client new_client(client_name, arrival_time);
        clients_[client_name] = new_client;

        return std::nullopt;
    }
    std::optional<Event> handle_client_sit(const Time &event_time, const std::string &event_body) {
        std::regex sit_regex("^([a-z0-9_-]+) (\\d+)$");
        std::smatch match;
        if (!std::regex_match(event_body, match, sit_regex)) {
            return Event(event_time, 13, "Error: invalid sit event body: <" + event_body + ">");
        }

        std::string client_name = match[1];
        int table_number = std::stoi(match[2]);

        if (!is_valid_table_number(table_number, tables_)) {
            return Event(event_time, 13, "Error: table number <" + std::to_string(table_number) + "> is out of range");
        }

        if (is_table_occupied(tables_, table_number)) {
            return Event(event_time, 13, "PlaceIsBusy");
        }

        if (!client_exists(clients_, client_name)) {
            return Event(event_time, 13, "ClientUnknown");
        }

        int table_index = table_number - 1;
        tables_[table_index].occupied = true;
        tables_[table_index].occupied_time_start = event_time;
        clients_[client_name].table_number = table_index;
        clients_[client_name].seated = true;

        return std::nullopt;
    }
    std::optional<Event> handle_client_start_waiting(const Time &event_time, const std::string &event_body) {
        const std::string &client_name = event_body;
        if (!is_valid_client_name(client_name)) {
            return Event(event_time, 13, "Invalid client name: " + client_name);
        }

        if (is_table_available(tables_)) {
            return Event(event_time, 13, "ICanWaitNoLonger!");
        }

        bool queue_at_full_capacity = waiting_list_.size() == tables_.size();
        if (queue_at_full_capacity) {
            Event leave_event(event_time, 11, client_name);
            return leave_event;
        }

        waiting_list_.push(clients_[client_name]);

        return std::nullopt;
    }
    std::optional<Event> handle_client_leave_table(const Time &event_time, const std::string &event_body) {
        const std::string &client_name = event_body;
        if (!is_valid_client_name(client_name)) {
            return Event(event_time, 13, "Invalid client name: " + client_name);
        }

        if (!client_exists(clients_, client_name)) {
            return Event(event_time, 13, "ClientUnknown");
        }

        Client &client = clients_[client_name];
        if (!client.seated) {
            return Event(event_time, 13, "Error: client " + client_name + " is not seated");
        }

        free_table(tables_[client.table_number], event_time, cost_per_hour_);

        clients_.erase(client_name);

        // find a client from the waiting list
        if (!waiting_list_.empty()) {
            Client next_client = waiting_list_.front();
            waiting_list_.pop();
            return Event(event_time, 12, next_client.name + " " + std::to_string(client.table_number + 1));
        }

        return std::nullopt;
    }
    void handle_client_leave(const Time &event_time, const std::string &event_body) {
        const std::string &client_name = event_body;
        if (!is_valid_client_name(client_name)) {
            return;
        }

        if (!client_exists(clients_, client_name)) {
            return;
        }

        Client &client = clients_[client_name];
        if (client.seated) {
            free_table(tables_[client.table_number], event_time, cost_per_hour_);
        }

        // if client was at waiting list, remove it, keeping the order
        waiting_list_ = remove_client_from_queue(waiting_list_, client_name);

        clients_.erase(client_name);
    }
    std::optional<Event> handle_event(Event &event) {
        std::optional<Event> new_event;

        switch (event.ID) {
            case 1:
                new_event = handle_client_arrival(event.time, event.body);
                break;
            case 2:
                new_event = handle_client_sit(event.time, event.body);
                break;
            case 3:
                new_event = handle_client_start_waiting(event.time, event.body);
                break;
            case 4:
                new_event = handle_client_leave_table(event.time, event.body);
                break;
            case 11:
                handle_client_leave(event.time, event.body);
                break;
            case 12:
                handle_client_sit(event.time, event.body);
                break;
            case 13:
                break;
        }

        return new_event;
    }
    void process_events(std::vector<Event> &events) {
        size_t i = 0;
        size_t events_size = events.size();
        while (i < events_size) {
            Event &event = events[i];
            std::cout << event << std::endl;

            if (event.time > end_time_) {
                Event late_event(end_time_, 13, "Error: event is after closing time");
                std::cout << late_event << std::endl;
                ++i;
                continue;
            }

            std::optional<Event> new_event = handle_event(event);

            if (new_event.has_value()) {
                event = new_event.value();
                if (i > 0) {
                    --i;
                } else {
                    continue;
                }
            }

            ++i;
        }

        std::vector<std::string> clients_to_leave;
        clients_to_leave.reserve(clients_.size());
        for (const auto &client: clients_) {
            clients_to_leave.push_back(client.first);
        }

        std::sort(clients_to_leave.begin(), clients_to_leave.end());

        for (const auto &client_name: clients_to_leave) {
            Event leave_event(end_time_, 11, client_name);
            handle_client_leave_table(leave_event.time, leave_event.body);
        }
    }

    void initialize_tables(int num_of_tables) {
        tables_.reserve(num_of_tables); // performance boost if N is big
        for (int i = 1; i <= num_of_tables; ++i) {
            tables_.emplace_back(i);
        }
    }
public:
    Computer_Club(const std::string &filename)
            : start_time_(0, 0), end_time_(0, 0) {
        int num_of_tables;
        int cost_per_hour;
        std::vector<Event> events;
        parse_input(filename, num_of_tables, start_time_, end_time_, cost_per_hour, events);

        cost_per_hour_ = cost_per_hour;

        initialize_tables(num_of_tables);
        process_events(events);
    }
    Time get_start_time() const {
        return start_time_;
    }
    Time get_end_time() const {
        return end_time_;
    }

    void print_tables() {
        for (const auto &table: tables_) {
            std::cout << table << std::endl;
        }
    }
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    Computer_Club club(argv[1]);

    std::cout << club.get_start_time() << std::endl;
    std::cout << club.get_end_time() << std::endl;
    club.print_tables();

    return 0;
}