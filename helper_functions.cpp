#include "helper_functions.h"

bool is_valid_client_name(const std::string& client_name)
{
    return std::regex_match(client_name, std::regex("^[a-zA-Z0-9_-]+$"));
}
bool client_exists(const std::unordered_map<std::string, Client>& clients, const std::string& client_name)
{
    return clients.find(client_name) != clients.end();
}
bool is_valid_table_number(int table_number, const std::vector<Table>& tables)
{
    return table_number >= 1 && table_number <= tables.size();
}
bool is_table_occupied(const std::vector<Table>& tables, int table_number)
{
    return tables[table_number - 1].occupied;
}
bool is_table_available(const std::vector<Table>& tables)
{
    for (const auto& table : tables) {
        if (!table.occupied) {
            return true;
        }
    }
    return false;
}
void free_table(Table& table, const Time& event_time, int cost_per_hour)
{
    table.occupied = false;
    table.occupied_time_end = event_time;
    Time client_occupied_for = table.occupied_time_end - table.occupied_time_start;
    table.total_occupied_time += client_occupied_for;
    int total_hours = client_occupied_for.hour + (client_occupied_for.minute > 0 ? 1 : 0);
    table.revenue += total_hours * cost_per_hour;
}
std::queue<Client> remove_client_from_queue(std::queue<Client>& waiting_list,
                                            const std::string& client_name)
{
    std::queue<Client> temp_queue;
    while (!waiting_list.empty()) {
        if (waiting_list.front().name != client_name) {
            temp_queue.push(waiting_list.front());
        }
        waiting_list.pop();
    }
    return temp_queue;
}
