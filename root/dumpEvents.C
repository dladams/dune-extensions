// dumpEvents.C
//
// David Adams
// October 2015
//
// Dumps the event tree from the current root file.
//
// E.g. to pring the event contents of mydata.root:
//   > root.exe mydata.root dumpEvents.C
// or
//   > root.exe 'dumpEvents.C("mydata.root")'

void dumpEvents(std::string fname="") {
  if ( fname.size() ) {
    TFile* pfile = TFile::Open(fname.c_str());
    if ( pfile == 0 || ! pfile->IsOpen() ) {
      cout << "ERROR: Unable to open input file " << fname << endl;
      return;
    }
  }
  if ( gFile == 0 ) {
    cout << "ERROR: Event file must be opened." << endl;
    return;
  }
  cout << "Dumping event content of " << gFile->GetName() << endl;
  TObject* pobj = gFile->Get("Events");
  if ( pobj == 0 ) {
    cout << "ERROR: Unable to find event tree." << endl;
    cout << "File contents: " << endl;
    pfile->ls();
    return;
  }
  pobj->Print();
}
