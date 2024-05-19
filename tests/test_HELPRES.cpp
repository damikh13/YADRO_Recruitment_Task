#include <gtest/gtest.h>
#include "../Computer_Club_STRUCTS.h"
#include "../helper_functions.h"

TEST(ValidClientName, name_with_lowercase_letters)
{
    ASSERT_TRUE(is_valid_client_name("client"));
}

TEST(ValidClientName, name_with_lowercase_lettersr2)
{
    ASSERT_FALSE(is_valid_client_name("CLIENT"));
}

TEST(ValidClientName, name_with_number)
{
    ASSERT_TRUE(is_valid_client_name("client1"));
}

TEST(ValidClientName, name_with_numberS)
{
    ASSERT_TRUE(is_valid_client_name("client42"));
}

TEST(ValidClientName, name_with_hyphen)
{
    ASSERT_TRUE(is_valid_client_name("client-name"));
}

TEST(ValidClientName, empty_name)
{
    ASSERT_FALSE(is_valid_client_name(""));
}

TEST(ValidClientName, name_with_special_characters)
{
    ASSERT_FALSE(is_valid_client_name("client@name"));
}

TEST(ValidClientName, name_with_space)
{
    ASSERT_FALSE(is_valid_client_name("cli ent"));
}

TEST(FreeTable, table_occupied_for_one_hour)
{
    Table table;
    table.occupied = true;
    table.occupied_time_start = Time { 10, 0 };
    free_table(table, Time(11, 0), 100);
    ASSERT_FALSE(table.occupied);
    ASSERT_EQ(table.occupied_time_end, Time(11, 0));
    ASSERT_EQ(table.total_occupied_time, Time(1, 0));
    ASSERT_EQ(table.revenue, 100);
}
TEST(FreeTable, table_occupied_for_half_hour)
{
    Table table;
    table.occupied = true;
    table.occupied_time_start = Time(10, 0);
    free_table(table, Time(10, 30), 100);
    ASSERT_FALSE(table.occupied);
    ASSERT_EQ(table.occupied_time_end, Time(10, 30));
    ASSERT_EQ(table.total_occupied_time, Time(0, 30));
    ASSERT_EQ(table.revenue, 100);
}
TEST(FreeTable, table_occupied_for_one_and_half_hour)
{
    Table table;
    table.occupied = true;
    table.occupied_time_start = Time { 10, 0 };
    free_table(table, Time(11, 30), 100);
    ASSERT_FALSE(table.occupied);
    ASSERT_EQ(table.occupied_time_end, Time(11, 30));
    ASSERT_EQ(table.total_occupied_time, Time(1, 30));
    ASSERT_EQ(table.revenue, 200);
}
TEST(FreeTable, table_not_occupied)
{
    Table table;
    table.occupied = false;
    table.occupied_time_start = Time(10, 0);
    free_table(table, Time(10, 0), 100);
    ASSERT_FALSE(table.occupied);
    ASSERT_EQ(table.occupied_time_end, Time(10, 0));
    ASSERT_EQ(table.total_occupied_time, Time(0, 0));
    ASSERT_EQ(table.revenue, 0);
}

TEST(RemoveClientFromQueue, client_in_queue)
{
    std::queue<Client> waiting_list;
    waiting_list.push(Client("First"));
    waiting_list.push(Client("Second"));
    waiting_list.push(Client("Third"));
    std::queue<Client> result = remove_client_from_queue(waiting_list, "Second");
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.front().name, "First");
    result.pop();
    ASSERT_EQ(result.front().name, "Third");
}

TEST(RemoveClientFromQueue, client_not_in_queue)
{
    std::queue<Client> waiting_list;
    waiting_list.push(Client("First"));
    waiting_list.push(Client("Second"));
    waiting_list.push(Client("Third"));

    std::queue<Client> result = remove_client_from_queue(waiting_list, "Fourth");
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.front().name, "First");
    result.pop();
    ASSERT_EQ(result.front().name, "Second");
    result.pop();
    ASSERT_EQ(result.front().name, "Third");
}

TEST(RemoveClientFromQueue, empty_queue) {
    std::queue<Client> waiting_list;
    std::queue<Client> result = remove_client_from_queue(waiting_list, "Alice");
    ASSERT_TRUE(result.empty());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}