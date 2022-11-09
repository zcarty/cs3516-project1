/*
* log.cpp
* Zeb Carty and Michael McInerney
*
* Includes functions that log user activity to admin file
*/
#include "include.h"

void log(string ip_address, string message) {
    time_t now = time(0);

    tm *local = localtime(&now);
    char buff[80];

    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", local);

    ofstream adminfile;
    adminfile.open("admin.txt", ios_base::app);
    adminfile << buff << " " << ip_address << " " << message << endl;
    adminfile.close();
}