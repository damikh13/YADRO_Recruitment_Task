#include "parsing_functions.h"

std::ifstream open_input_file(const std::string& filename)
{
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        throw std::runtime_error("Error: cannot open input file <" + filename + ">");
    }
    return input_file;
}
int parse_num_of_tables(std::ifstream& input_file)
{
    std::string line;
    std::getline(input_file, line);
    return std::stoi(line);
}
Time parse_time(const std::string& time_str)
{
    std::regex time_regex("^([01]?[0-9]|2[0-3]):[0-5][0-9]$");
    if (!std::regex_match(time_str, time_regex)) {
        throw std::runtime_error(
            "Invalid time format: " + time_str);
    }
    std::string hour_str = time_str.substr(0, 2);
    std::string minute_str = time_str.substr(3, 2);
    int hour = std::stoi(hour_str);
    int minute = std::stoi(minute_str);
    return { hour, minute };
}
int parse_cost_per_hour(std::ifstream& input_file)
{
    std::string line;
    std::getline(input_file, line);
    return std::stoi(line);
}
std::vector<Event> parse_events(std::istream& input_stream)
{
    std::vector<Event> events;
    std::regex event_regex(
        R"((\d{2}:\d{2}) (\d+) (.+))"); // 09:00 4 client1
    std::smatch match;
    std::string line;
    while (std::getline(input_stream, line)) {
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
void parse_input(const std::string& filename, int& num_of_tables, Time& start_time, Time& end_time, int& cost_per_hour, std::vector<Event>& events)
{
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
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
}
