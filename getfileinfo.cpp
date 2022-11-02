#include<iostream>
#include<fstream>
using namespace std;

int main(string path) {
   ifstream in_file(path, ios::binary);
   in_file.seekg(0, ios::end);
   int file_size = in_file.tellg();
   cout<<"Size of the file is"<<" "<< file_size<<" "<<"bytes";

   return file_size;
}