#include "include.h"

/* From file location of a QR Code, produces url*/
string getURL(string fileLoc)
{
    string start = "java -cp javase.jar:core.jar com.google.zxing.client.j2se.CommandLineRunner " + fileLoc;
    const char * command = start.c_str();
    
    system(command);
}