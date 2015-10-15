// dune_extensions/root/rootlogon.C
//
// David Adams
// October 2015
//
// Root logon file to load the dune_extensions macros.
//
// Dune, art_extensions and dune_extensions products
// must be set up, e.g.
//   setup dune_extensions v00_00_03 -q e7:prof

{
  gSystem->AddIncludePath("-I$BOOST_INC");
  //gSystem->AddIncludePath("-DBOOST_NO_CWCHAR");
  gSystem->AddIncludePath("-I$CPP0X_INC");
  gSystem->AddIncludePath("-I$CETLIB_INC");
  gSystem->AddIncludePath("-I$FHICLCPP_INC");
  gSystem->AddIncludePath("-I$CLHEP_INC");
  gSystem->AddIncludePath("-I$LARCORE_INC");
  gSystem->AddIncludePath("-I$DUNETPC_INC");
  gSystem->AddIncludePath("-I$ART_EXTENSIONS_INC");
  gSystem->AddIncludePath("-I$DUNE_EXTENSIONS_INC");
  gSystem->AddDynamicPath("-L$FHICLCPP_LIB -lfhiclcpp");
  gSystem->AddLinkedLibs("$CETLIB_LIB/libcetlib.so");
  gSystem->AddLinkedLibs("$FHICLCPP_LIB/libfhiclcpp.so");
  gSystem->AddLinkedLibs("$LARCORE_LIB/libGeometry.so");
  gSystem->AddLinkedLibs("$DUNETPC_LIB/libdune_Geometry.so");
  gSystem->AddLinkedLibs("$DUNE_EXTENSIONS_LIB/libDXUtil.so");
  gSystem->AddLinkedLibs("$DUNE_EXTENSIONS_LIB/libDXGeometry.so");
  string ifname = "perf.root";
  TFile* pfile = TFile::Open(ifname.c_str());
  if ( pfile == 0 || ! pfile.IsOpen() ) {
    cout << "Input file not found: " << ifname << endl;
  } else {
    pfile->cd("DXDisplay");
  }
  gROOT->ProcessLine(".L palette.cxx+");
  gROOT->ProcessLine(".L gettree.cxx+");
  gROOT->ProcessLine(".L draw.cxx+");
  gROOT->ProcessLine(".L drawpars.cxx+");
  draw::set35t();
  draw::set10ktw();
  //gROOT->ProcessLine(".L detlar.cxx+");
  gROOT->ProcessLine(".L drawTracks.C");
  gStyle->SetPadRightMargin(0.14);   // For 2D plots
  if ( mcptree("McParticleTree") ) {
    mcptree()->SetMarkerStyle(2);
  }
}
