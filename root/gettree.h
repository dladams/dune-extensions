// gettree.h

// David Adams
// October 2011
//
// Functions to locate trees in the current Root directory.

#include <string>

class TTree;

// Return the named tree if it exists.
// The name "null" returns 0.
TTree* gettree(std::string tname);

// Find MC particle tree.
TTree* mcptree(std::string tname ="");

// Find MC performance tree.
TTree* perftree(std::string tname ="");
