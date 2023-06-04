#include "wal/wal_writer.h"

#include <iostream>
#include <string_view>

using namespace std;
using namespace tinykv;
using namespace tinykv::wal;

std::string Dirname1(const std::string &filename) {
  auto pos = filename.rfind('/');
  std::string dirname{};
  if (pos != std::string::npos) {
    return filename.substr(0, pos);
  }
  return ".";
}

int main() {
  std::cout << __FILE__ << endl;
  std::cout << "Beginning the test of write ahead log...\n";
  const std::string dirname = Dirname1(__FILE__);
  std::cout << dirname << endl;
  const std::string filename = dirname + "/wal_test.wal";
  tinykv::WritableFile* dest = new CommonWritableFile(filename);
  Writer wal_writer(dest);

  wal_writer.AddRecord("shaoguixin alaways like the ice, lalalalalala....");
  std::cout << "Finished the writing..\n";
}
