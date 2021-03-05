#include <iostream>
#include <fstream>
//#include <sstream>

using namespace std;

int main()
{
  char chr;
  ifstream file;
  file.open("loremipsum.txt");

  int counter = 0;

  while (file.get(chr)) {
    if (chr == 'a') {
      counter++;
    }
  }
  cout << counter << endl;

  return 0;
}

/*
string line;
while (getline(file, line)) {
  istringstream iss(line);

  for (unsigned int i = 0; i < line.length(); i++) {
    if (line[i] == 'a') {
      counter++;
    }
  }
}
*/
