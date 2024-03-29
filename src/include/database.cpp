#include "../../include/speedyGo.hpp"
#include "../../include/database.hpp"
#include "../../include/declarations.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>

sql::Driver     *driver = nullptr;
sql::Connection *con    = nullptr;

bool fetchTables(void)
{
    std::cout << std::setw(5) << " "
              << "-- Connecting..."; 

    try
    {
        if (!__MySQL_CONNECTED)
        {

            driver = sql::mysql::get_mysql_driver_instance();

            con = driver->connect("tcp://" + std::string(AUTHENTICATION::getNode()->host_name),
                                  std::string(AUTHENTICATION::getNode()->username),
                                  std::string(AUTHENTICATION::getNode()->password));

            if (con == nullptr)
                return EXIT_FAILURE;

            con->setSchema(AUTHENTICATION::getNode()->database);

            __MySQL_CONNECTED = true;
        }

        VERTEX_COUNT = 0;

        if (__DATABASE_RETRIEVED)
        {
            depot.clear();
            product.clear();
            city.clear();
        }

        depot.fetchTable();
        product.fetchTable();
        city.fetchTable();

        __DATABASE_RETRIEVED = true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "\n"
                  << std::setw(5) << " "
                  << "Error code: " << e.getErrorCode() << "\n"
                  << std::setw(5) << " "
                  << "Error message: " << e.what() << "\n"
                  << std::setw(5) << " "
                  << "SQLState: " << e.getSQLState() << "\n";

        return EXIT_FAILURE;
    }

    std::cout << "\n";

    return EXIT_SUCCESS;
}

bool addGraphEdge(void)
{
    clear_screen();
    std::cout << "\n\n";

    CITY::CITY_NODE::displayCityData();

    std::cout << std::setw(5) << " "
              << "Enter the city edges ('0' to return):\n";

    underline(80, true);

    std::ofstream edgeFile;
    edgeFile.open(EDGES, std::ios::out | std::ios::app);
    if (!edgeFile.is_open())
    {
        std::cerr << std::setw(5) << " "
                  << "--> Failed to open the edge file!\n";
        _getch();
        return EXIT_FAILURE;
    }

    std::cin.ignore(9999, '\n');

    while (true)
    {
        std::string edge;
        int City_ID = -1;
        bool isFound = false;

        std::cout << std::setw(5) << " " << newCityName << " <--> ";
        std::getline(std::cin, edge);

        if (_strcasecmp_(edge.c_str(), "0") == 0)
            break;

        for (CITY::CITY_NODE *date_oras = city.getHead(); date_oras != nullptr; date_oras = date_oras->next)
            if (_strcasecmp_(date_oras->getCityName(), edge.c_str()) == 0)
            {
                isFound = true;
                City_ID = std::stoi(date_oras->getCityID());
                break;
            }

        if (isFound)
            edgeFile << VERTEX_COUNT - 1 << " " << City_ID << "\n";
        else
            std::cerr << std::setw(5) << " "
                      << "Invalid edge!\n\n";
    }

    edgeFile.close();

    speedyGo::_ADJACENCY_MATRIX_INIT_();

    return EXIT_SUCCESS;
}