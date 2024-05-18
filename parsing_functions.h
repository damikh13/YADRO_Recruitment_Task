#ifndef RECRUITMENT_TEST_PARSING_FUNCTIONS_H
#define RECRUITMENT_TEST_PARSING_FUNCTIONS_H

#include "Computer_Club_STRUCTS.h" // Time, Event, Client, Table
#include <fstream>
#include <string>
#include <vector>

std::ifstream open_input_file(const std::string& filename);
int parse_num_of_tables(std::ifstream& input_file);
Time parse_time(const std::string& time_str);
int parse_cost_per_hour(std::ifstream& input_file);
std::vector<Event> parse_events(std::istream& input_stream);
void parse_input(const std::string& filename, int& num_of_tables,
    Time& start_time, Time& end_time, int& cost_per_hour,
    std::vector<Event>& events);

#endif // RECRUITMENT_TEST_PARSING_FUNCTIONS_H
