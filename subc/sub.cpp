
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class item {
public:
    item(string t1, string t2) : time(t1), text(t2) {}
    string time;
    string text;
};

vector<item> items;

int main(int argc, char *argv[])
{
   if(argc < 3) {
       cout << "Usage:" << endl;
       cout << "sub file file" << endl;
       exit(1);
   }

   fstream file;
   file.open(argv[1], ios::in);
   if(!file.is_open()) {
       cout << "cant open file" << endl;
       exit(1);
   }

  string time;
  string text;
  for(;;) {
      if(!getline(file, time)) break;
      if(!getline(file, text)) break;
      items.push_back(item(time, text));
  }
  file.close();

  fstream ofile;
  ofile.open(argv[2], ios::out);
  if(!ofile.is_open()) {
      cout << "cant open output file" << endl;
      exit(1);
  }
  for(int i=0; i < items.size(); i++) {
      ofile << (i+1) << endl;
      ofile << "00:" << items[i].time << ",000 --> 00:" << items[i < items.size()-1 ? i+1 : items.size()-1].time << ",000" << endl;
      ofile << items[i].text << endl << endl;
  }
  ofile.close();
}
