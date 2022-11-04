#include "include.h"

/* Take command and return terminal output */
string exec(string command) {
   char buff[128];
   string output = "";

   // Create a new stream with popen
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed.";
   }

   // Send data to buffer until end is reached
   while (!feof(pipe)) {
      if (fgets(buff, sizeof(buff), pipe) != NULL)
         output += buff;
   }

   pclose(pipe);
   return output;
}


/* From file location of a QR Code, produces url*/
string getURL(string fileLoc)
{
    string command = "java -cp javase.jar:core.jar com.google.zxing.client.j2se.CommandLineRunner " + fileLoc;
    
    return exec(command);
}