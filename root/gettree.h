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

// Following functions return pointers to trees and remember the
// name so that a subsequent call can be made without a name.
// The name "null" can be used to reset the pointer to null.
TTree* mcptree(std::string tname ="");
