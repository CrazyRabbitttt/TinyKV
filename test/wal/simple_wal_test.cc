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
  const std::string filename = dirname + "/wal_test_file.txt";
//  const std::string tmp_filename = dirname + "/tmp_test";
  tinykv::WritableFile* dest = new CommonWritableFile(filename);
  Writer wal_writer(dest);

//  std::string tmp{"shaoguixin alaways like the ice, lalalalalala...."};
//  wal_writer.DumpRecordToPhysical(RecordType(tinykv::wal::kFullType), tmp.data(), tmp.length());
  wal_writer.AddRecord("shaoguixin alaways like the ice, hahahahhahaha....");
  std::cout << "Finished the writing..\n";
}
