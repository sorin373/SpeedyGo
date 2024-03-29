#include "../../../include/classes/depot.hpp"
#include "../../../include/declarations.hpp"
#include "../../../include/database.hpp"

#include <iostream>
#include <iomanip>
#ifdef _WIN32
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/statement.h>
#elif __linux__
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#endif

DEPOT::DEPOT_NODE::DEPOT_NODE(const char *Product_ID, const char *City_ID, const double Product_Quantity)
{
    this->Product_ID = strdup(Product_ID);
    this->City_ID = strdup(City_ID);
    this->Product_Quantity = Product_Quantity;
    prev = nullptr;
    next = nullptr;
}

DEPOT::DEPOT_NODE::~DEPOT_NODE(void)
{
    free(Product_ID);
    free(City_ID);
}

char *DEPOT::DEPOT_NODE::getProductID(void) const noexcept
{
    return Product_ID;
}

char *DEPOT::DEPOT_NODE::getCityID(void) const noexcept
{
    return City_ID;
}

double DEPOT::DEPOT_NODE::getProductQuantity(void) const noexcept
{
    return Product_Quantity;
}

DEPOT::DEPOT_NODE *DEPOT::getHead(void) const noexcept
{
    return head_depot;
}

DEPOT::DEPOT_NODE *DEPOT::getTail(void) const noexcept
{
    return tail_depot;
}

void DEPOT::getData(const char *Product_ID, const char *City_ID, const double Product_Quantity)
{
    DEPOT_NODE *newnod = new DEPOT_NODE(Product_ID, City_ID, Product_Quantity);

    if (head_depot == nullptr)
    {
        head_depot = newnod;
        tail_depot = newnod;
    }
    else
    {
        tail_depot->next = newnod;
        newnod->prev = tail_depot;
        tail_depot = newnod;
    }
}

void DEPOT::DEPOT_NODE::swapData(DEPOT_NODE &node)
{
    std::swap(Product_ID, node.Product_ID);
    std::swap(City_ID, node.City_ID);
    std::swap(Product_Quantity, node.Product_Quantity);
}

void DEPOT::DEPOT_NODE::sortData(void)
{
    bool isSorted = true;

    DEPOT_NODE *ptr = nullptr, *l_ptr = nullptr;

    if (depot.getHead() == nullptr)
        return;

    do
    {
        isSorted = true;
        ptr = depot.getHead();

        while (ptr->next != l_ptr)
        {
            if (std::stoi(ptr->getProductID()) > std::stoi(ptr->next->getProductID()))
            {
                ptr->swapData(*(ptr->next));

                isSorted = false;
            }
            ptr = ptr->next;
        }
        l_ptr = ptr;
    } while (!isSorted);
}

void DEPOT::DEPOT_NODE::searchLimitedStockCities(void)
{
    limited_stock_cities_count = 0;
    CITY::CITY_NODE *city_data = city.getHead();
    while (city_data != nullptr)
    {
        int CityID = std::stoi(city_data->getCityID());

        DEPOT::DEPOT_NODE *depot_data = depot.getHead();
        while (depot_data != nullptr)
        {
            int depotID = std::stoi(depot_data->getCityID());
            if (depotID == CityID && centralDepos[depotID] == false)
                if (depot_data->getProductQuantity() < MINIMUM_STOCK_VALUE)
                {
                    limitedStockCities[depotID] = true;
                    limited_stock_cities_count++;
                    break;
                }
            depot_data = depot_data->next;
        }
        city_data = city_data->next;
    }
}

void DEPOT::DEPOT_NODE::updateQuantity(const double newQuantity)
{
    Product_Quantity = newQuantity;
}

bool DEPOT::refreshData(void)
{
    try
    {
        std::string deleteQuery = "DELETE FROM depozit";
        sql::Statement *stmt = con->createStatement();

        stmt->executeUpdate(deleteQuery);
        delete stmt;

        std::string insertQuery = "INSERT INTO depozit (ID_Produs, ID_Oras, Cantitate_Produs) VALUES (?, ?, ?)";
        sql::PreparedStatement *prepStmt = con->prepareStatement(insertQuery);

        for (DEPOT::DEPOT_NODE *date_depozit = head_depot; date_depozit != nullptr; date_depozit = date_depozit->next)
        {
            int Product_ID = std::stoi(date_depozit->getProductID()),
                City_ID = std::stoi(date_depozit->getCityID()),
                Product_Quantity = date_depozit->getProductQuantity();

            prepStmt->setInt(1, Product_ID);
            prepStmt->setInt(2, City_ID);
            prepStmt->setInt(3, Product_Quantity);

            prepStmt->executeUpdate();
        }

        delete prepStmt;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "\n"
                  << std::setw(5) << " "
                  << "-- Error code: " << e.getErrorCode() << "\n"
                  << std::setw(5) << " "
                  << "-- Error message: " << e.what() << "\n"
                  << std::setw(5) << " "
                  << "-- SQLState: " << e.getSQLState() << "\n";

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void DEPOT::fetchTable(void)
{
    sql::Statement *stmt = nullptr;
    sql::ResultSet *res = nullptr;

    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM depozit");

    while (res->next())
    {
        int iProduct_ID = res->getInt("ID_Produs");
        char *cProduct_ID = (char *)malloc(std::to_string(iProduct_ID).length() + 1);
        strcpy(cProduct_ID, std::to_string(iProduct_ID).c_str());

        int iCity_ID = res->getInt("ID_Oras");
        char *cCity_ID = (char *)malloc(std::to_string(iCity_ID).length() + 1);
        strcpy(cCity_ID, std::to_string(iCity_ID).c_str());

        double Prdouct_Quantity = res->getDouble("Cantitate_Produs");

        depot.getData(cProduct_ID, cCity_ID, Prdouct_Quantity);

        free(cProduct_ID);
        free(cCity_ID);
    }

    res->close();
    stmt->close();
    delete res;
    delete stmt;
}

void DEPOT::clear()
{
    while (head_depot != nullptr)
    {
        DEPOT_NODE *temp = head_depot;
        head_depot = head_depot->next;
        delete temp;
    }

    tail_depot = nullptr;
}

DEPOT::~DEPOT()
{
    DEPOT_NODE *ptr = head_depot;

    while (ptr != nullptr)
    {
        DEPOT_NODE *temp = ptr;
        ptr = ptr->next;
        delete temp;
    }

    head_depot = nullptr;
    tail_depot = nullptr;
}