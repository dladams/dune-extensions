// dune_extensions/rootscripts/client_rootlogon.C
//
// The dxroot command copies this file to ./rootlogon.C.
//
// Relevant changes should be propagated back.

{
  gROOT->ProcessLine(".X $DUNE_EXTENSIONS_ROOTSCRIPTS/rootlogon_load.C");
  gROOT->ProcessLine(".X $DUNE_EXTENSIONS_ROOTSCRIPTS/rootlogon_setup.C");
  gROOT->ProcessLine(".L drawrms.cxx+");
  gROOT->ProcessLine(".L drawstuck.cxx+");
}
