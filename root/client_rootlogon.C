// dune_extensions/rootscripts/client_rootlogon.C
//
// The dxroot command copies this file to ./rootlogon.C.
//
// Relevant changes should be propagated back.

{
  int dbg = 1;
  if ( dbg > 0 ) cout << "Welcome to DXRoot." << endl;
  string mpath = TROOT::GetMacroPath();
  mpath += "$DUNE_EXTENSIONS_ROOTSCRIPTS:";
  if ( dbg > 1 ) cout << "Macro path: " << mpath << endl;
  TROOT::SetMacroPath(mpath.c_str());
  gSystem->AddIncludePath("-I$DUNE_EXTENSIONS_ROOTSCRIPTS");
  if ( dbg > 1 ) cout << "Include path: " << gSystem->GetIncludePath() << endl;
  gROOT->ProcessLine(".X $DUNE_EXTENSIONS_ROOTSCRIPTS/rootlogon.C");
}
