#include "Computer_Club.h"

int main(int argc, char* argv[])
{
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
