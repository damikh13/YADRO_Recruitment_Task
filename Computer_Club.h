#ifndef RECRUITMENT_TEST_COMPUTER_CLUB_H
#define RECRUITMENT_TEST_COMPUTER_CLUB_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <optional>
#include "Computer_Club_STRUCTS.h" // Time, Event, Client, Table

class Computer_Club {
private:
    std::unordered_map<std::string, Client> clients_;
    std::vector<Table> tables_;
    std::queue<Client> waiting_list_;
    Time start_time_;
    Time end_time_;
    int cost_per_hour_;

    std::optional<Event> handle_client_arrival_(const Time& arrival_time, const std::string& event_body);
    std::optional<Event> handle_client_sit_(const Time& event_time, const std::string& event_body);
    std::optional<Event> handle_client_start_waiting_(const Time& event_time, const std::string& event_body);
    std::optional<Event> handle_client_leave__table_(const Time& event_time, const std::string& event_body);
    void handle_client_leave_(const Time& event_time, const std::string& event_body);

    std::optional<Event> handle_event_(Event& event);
    void process_events_(std::vector<Event>& events);
    void initialize_tables_(int num_of_tables);

public:
    Computer_Club(const std::string& filename);
    Time get_start_time() const { return start_time_; }
    Time get_end_time() const { return end_time_; }
    void print_tables();
};

#endif // RECRUITMENT_TEST_COMPUTER_CLUB_H
