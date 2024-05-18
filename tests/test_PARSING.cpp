#include <gtest/gtest.h>
#include "parsing_functions.h"

TEST(ParseTime, valid_time_format) {
    std::string time_str = "12:30";
    Time result = parse_time(time_str);
    ASSERT_EQ(result.hour, 12);
    ASSERT_EQ(result.minute, 30);
}

TEST(ParseTime, invalid_time_format) {
    std::string time_str = "1230";
    ASSERT_THROW(parse_time(time_str), std::runtime_error);
}

TEST(ParseTime, hour_out_of_range) {
    std::string time_str = "24:30";
    ASSERT_THROW(parse_time(time_str), std::runtime_error);
}

TEST(ParseTime, minute_out_of_range) {
    std::string time_str = "12:60";
    ASSERT_THROW(parse_time(time_str), std::runtime_error);
}

TEST(ParseTime, leading_zero_in_hour) {
    std::string time_str = "09:30";
    Time result = parse_time(time_str);
    ASSERT_EQ(result.hour, 9);
    ASSERT_EQ(result.minute, 30);
}

TEST(ParseTime, leading_zero_in_minute) {
    std::string time_str = "12:05";
    Time result = parse_time(time_str);
    ASSERT_EQ(result.hour, 12);
    ASSERT_EQ(result.minute, 5);
}

TEST(ParseTime, leading_zero_in_hour_and_minute) {
    std::string time_str = "09:05";
    Time result = parse_time(time_str);
    ASSERT_EQ(result.hour, 9);
    ASSERT_EQ(result.minute, 5);
}

TEST(ParseTime, no_leading_zero) {
    std::string time_str = "9:5";
    ASSERT_THROW(parse_time(time_str), std::runtime_error);
}

TEST(ParseEvents, valid_events_format) {
        std::istringstream input_file("09:00 4 client1\n18:00 12 client2 4\n");
        std::vector<Event> events = parse_events(input_file);
        ASSERT_EQ(events.size(), 2);
        ASSERT_EQ(events[0].time.hour, 9);
        ASSERT_EQ(events[0].time.minute, 0);
        ASSERT_EQ(events[0].ID, 4);
        ASSERT_EQ(events[0].body, "client1");
        ASSERT_EQ(events[1].time.hour, 18);
        ASSERT_EQ(events[1].time.minute, 0);
        ASSERT_EQ(events[1].ID, 12);
        ASSERT_EQ(events[1].body, "client2 4");
}

TEST(ParseEvents, invalid_events_format) {
    std::istringstream input_file("9:00 client1\n18:00 client2 4\n");
    std::vector<Event> events = parse_events(input_file);
    ASSERT_TRUE(events.empty());
}

TEST(ParseEvents, invalid_name) {
    std::istringstream input_file("09:00 !client!");
    std::vector<Event> events = parse_events(input_file);
    ASSERT_TRUE(events.empty());
}

TEST(ParseEvents, signle_event) {
    std::istringstream input_file("09:00 4 client_ 123\n");
    std::vector<Event> events = parse_events(input_file);
    ASSERT_EQ(events.size(), 1);
    ASSERT_EQ(events[0].time.hour, 9);
    ASSERT_EQ(events[0].time.minute, 0);
    ASSERT_EQ(events[0].ID, 4);
    ASSERT_EQ(events[0].body, "client_ 123");
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}