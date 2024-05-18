#ifndef RECRUITMENT_TEST_HELPER_FUNCTIONS_H
#define RECRUITMENT_TEST_HELPER_FUNCTIONS_H

#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include "Computer_Club_STRUCTS.h" // Time, Event, Client, Table

bool is_valid_client_name(const std::string& client_name);
bool client_exists(const std::unordered_map<std::string, Client>& clients, const std::string& client_name);
bool is_valid_table_number(int table_number, const std::vector<Table>& tables);
bool is_table_occupied(const std::vector<Table>& tables, int table_number);
bool is_table_available(const std::vector<Table>& tables);
void free_table(Table& table, const Time& event_time, int cost_per_hour);
std::queue<Client> remove_client_from_queue(std::queue<Client>& waiting_list, const std::string& client_name);

#endif // RECRUITMENT_TEST_HELPER_FUNCTIONS_H
