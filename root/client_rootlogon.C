// dune_extensions/rootscripts/client_rootlogon.C
//
// The dxroot command copies this file to ./rootlogon.C.
//
// Relevant changes should be propagated back.

{
  cout << "Welcome to DXRoot." << endl;
  string mpath = TROOT::GetMacroPath();
  mpath += "$DUNE_EXTENSIONS_ROOTSCRIPTS:";
  cout << "Macro path: " << mpath << endl;
  TROOT::SetMacroPath(mpath.c_str());
  gSystem->AddIncludePath("-I$DUNE_EXTENSIONS_ROOTSCRIPTS");
  cout << "Include path: " << gSystem->GetIncludePath() << endl;
  gROOT->ProcessLine(".X $DUNE_EXTENSIONS_ROOTSCRIPTS/rootlogon.C");
}
