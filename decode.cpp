#include "include.h"

/* Take command and return terminal output*/
string exec(string command) {
   char buffer[128];
   string result = "";

   // Open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed!";
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   pclose(pipe);
   return result;
}


/* From file location of a QR Code, produces url*/
string getURL(string fileLoc)
{
    string command = "java -cp javase.jar:core.jar com.google.zxing.client.j2se.CommandLineRunner " + fileLoc;
    
    return exec(command);
}