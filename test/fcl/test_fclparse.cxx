// test_fclparse.cxx

// David Adams
// September 2015
//
// Test script for using fcl tools.

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "cetlib/filepath_maker.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using std::ofstream;
using fhicl::ParameterSet;
using fhicl::make_ParameterSet;
using cet::filepath_maker;
using cet::filepath_lookup;

void checkcfg(const ParameterSet& cfg) {
  const string myname = "checkcfg: ";
  assert( cfg.has_key("source") );
  assert( cfg.is_key_to_table("source") );
  ParameterSet source;
  assert( ! cfg.get_if_present<ParameterSet>("badsource", source) );
  assert( source.is_empty() );
  assert( cfg.get_if_present<ParameterSet>("source", source) );
  assert( ! source.is_empty() );
  source = cfg.get<ParameterSet>("source");
  assert( ! source.is_empty() );
  cout << myname << "Name: " << source.get<string>("Name") << endl;
  cout << myname << "Year: " << source.get<int>("Year") << endl;
  assert( source.get<string>("Name") == "DUNE" );
  assert( source.get<int>("Year") == 2015 );
  assert( source.get<float>("Year") == 2015 );
}

int main() {
  const string myname = "test_fclparse: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  string line = "-----------------------------";

  cout << myname << line << endl;
  cout << myname << line << endl;
  cout << myname << "Create config string." << endl;
  string scfg = "source: { Name: \"DUNE\" Year: 2015 }";
  cout << myname << scfg << endl;
  assert( scfg.size() > 0 );

  cout << myname << line << endl;
  cout << myname << "Convert string to fcl." << endl;
  ParameterSet cfg1;
  make_ParameterSet(scfg, cfg1);
  checkcfg(cfg1);

  cout << myname << line << endl;
  cout << myname << "Create file." << endl;
  system("ls -ls");
  {
    ofstream fout("fclparse.tmp");
    fout << scfg << endl;
  }
  system("ls -ls");
  system("cat fclparse.tmp");

  cout << myname << line << endl;
  cout << myname << "Get FCL path." << endl;
  string path = getenv("FHICL_FILE_PATH");
  cout << myname << "FCL path: " << path << endl;
  assert( path.size() != 0 );

  cout << myname << line << endl;
  cout << myname << "Convert file to fcl." << endl;
  filepath_lookup fpm(path);
  ParameterSet cfg2;
  make_ParameterSet("fclparse.tmp", fpm, cfg2);
  checkcfg(cfg2);

  cout << myname << line << endl;
  cout << myname << "Remove file." << endl;
  system("rm -f fclparse.tmp");

  cout << myname << line << endl;
  cout << myname << "Done." << endl;
  return 0;
}
