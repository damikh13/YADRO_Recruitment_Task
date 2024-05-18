#include "Computer_Club.h"
#include "helper_functions.h"
#include "parsing_functions.h"

std::optional<Event> Computer_Club::handle_client_arrival_(const Time& arrival_time, const std::string& event_body)
{
    const std::string& client_name = event_body;
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
std::optional<Event> Computer_Club::handle_client_sit_(const Time& event_time, const std::string& event_body)
{
    std::regex sit_regex("^([a-z0-9_-]+) (\\d+)$");
    std::smatch match;
    if (!std::regex_match(event_body, match, sit_regex)) {
        return Event(event_time, 13,
            "Error: invalid sit event body: <" + event_body + ">");
    }

    std::string client_name = match[1];
    int table_number = std::stoi(match[2]);

    if (!is_valid_table_number(table_number, tables_)) {
        return Event(event_time, 13,
            "Error: table number <" + std::to_string(table_number) + "> is out of range");
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
std::optional<Event> Computer_Club::handle_client_start_waiting_(const Time& event_time, const std::string& event_body)
{
    const std::string& client_name = event_body;
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
std::optional<Event> Computer_Club::handle_client_leave__table_(const Time& event_time, const std::string& event_body)
{
    const std::string& client_name = event_body;
    if (!is_valid_client_name(client_name)) {
        return Event(event_time, 13, "Invalid client name: " + client_name);
    }

    if (!client_exists(clients_, client_name)) {
        return Event(event_time, 13, "ClientUnknown");
    }

    Client& client = clients_[client_name];
    if (!client.seated) {
        return Event(event_time, 13,
            "Error: client " + client_name + " is not seated");
    }

    free_table(tables_[client.table_number], event_time, cost_per_hour_);

    clients_.erase(client_name);

    // find a client from the waiting list
    if (!waiting_list_.empty()) {
        Client next_client = waiting_list_.front();
        waiting_list_.pop();
        return Event(
            event_time, 12,
            next_client.name + " " + std::to_string(client.table_number + 1));
    }

    return std::nullopt;
}
void Computer_Club::handle_client_leave_(const Time& event_time, const std::string& event_body)
{
    const std::string& client_name = event_body;
    if (!is_valid_client_name(client_name)) {
        return;
    }

    if (!client_exists(clients_, client_name)) {
        return;
    }

    Client& client = clients_[client_name];
    if (client.seated) {
        free_table(tables_[client.table_number], event_time, cost_per_hour_);
    }

    // if client was at waiting list, remove it, keeping the order
    waiting_list_ = remove_client_from_queue(waiting_list_, client_name);

    clients_.erase(client_name);
}
std::optional<Event> Computer_Club::handle_event_(Event& event)
{
    std::optional<Event> new_event;

    switch (event.ID) {
    case 1:
        new_event = handle_client_arrival_(event.time, event.body);
        break;
    case 2:
        new_event = handle_client_sit_(event.time, event.body);
        break;
    case 3:
        new_event = handle_client_start_waiting_(event.time, event.body);
        break;
    case 4:
        new_event = handle_client_leave__table_(event.time, event.body);
        break;
    case 11:
        handle_client_leave_(event.time, event.body);
        break;
    case 12:
        handle_client_sit_(event.time, event.body);
        break;
    case 13:
        break;
    }

    return new_event;
}
void Computer_Club::process_events_(std::vector<Event>& events)
{
    size_t i = 0;
    size_t events_size = events.size();
    while (i < events_size) {
        Event& event = events[i];
        std::cout << event << std::endl;

        if (event.time > end_time_) {
            Event late_event(end_time_, 13, "Error: event is after closing time");
            std::cout << late_event << std::endl;
            ++i;
            continue;
        }

        std::optional<Event> new_event = handle_event_(event);

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
    for (const auto& client : clients_) {
        clients_to_leave.push_back(client.first);
    }

    std::sort(clients_to_leave.begin(), clients_to_leave.end());

    for (const auto& client_name : clients_to_leave) {
        Event leave_event(end_time_, 11, client_name);
        handle_client_leave__table_(leave_event.time, leave_event.body);
    }
}
void Computer_Club::initialize_tables_(int num_of_tables)
{
    tables_.reserve(num_of_tables); // performance boost if N is big
    for (int i = 1; i <= num_of_tables; ++i) {
        tables_.emplace_back(i);
    }
}
Computer_Club::Computer_Club(const std::string& filename)
    : start_time_(0, 0)
    , end_time_(0, 0)
{
    int num_of_tables;
    int cost_per_hour;
    std::vector<Event> events;
    parse_input(filename, num_of_tables, start_time_, end_time_, cost_per_hour,
        events);

    cost_per_hour_ = cost_per_hour;

    initialize_tables_(num_of_tables);
    process_events_(events);
}
void Computer_Club::print_tables()
{
    for (const auto& table : tables_) {
        std::cout << table << std::endl;
    }
}
