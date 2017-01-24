// dune_extensions/root/rootlogon_load.C
//
// David Adams
// July 2016.
//
// Root logon file to load the dune classes and dune_extensions classes.
//
// Dune, art_extensions and dune_extensions products
// must be set up, e.g.
//   setup dune_extensions v06_00_01 -q e10:prof
//
// Global symbol definitions should be put in rootlogon_setup.C.

{
  cout << "Welcome to DXRoot." << endl;

  string mpath = TROOT::GetMacroPath();
  mpath += "$DUNE_EXTENSIONS_ROOTSCRIPTS:";
  TROOT::SetMacroPath(mpath.c_str());
  gSystem->AddIncludePath("-I$DUNE_EXTENSIONS_ROOTSCRIPTS");
  gSystem->SetBuildDir(".aclic");

  gSystem->AddIncludePath("-I$BOOST_INC");
  gSystem->AddIncludePath("-I$CETLIB_INC");
  gSystem->AddIncludePath("-I$FHICLCPP_INC");
  gSystem->AddIncludePath("-I$CLHEP_INC");
  gSystem->AddIncludePath("-I$LARCORE_INC");
  gSystem->AddIncludePath("-I$LAREVT_INC");
  gSystem->AddIncludePath("-I$DUNETPC_INC");
  //gSystem->AddIncludePath("-I$DUNE_EXTENSIONS_INC");

  gSystem->AddDynamicPath("-L$FHICLCPP_LIB -lfhiclcpp");

  // Add linked libs.
  vector<string> libs;
  libs.push_back("$CETLIB_LIB/libcetlib");
  libs.push_back("$FHICLCPP_LIB/libfhiclcpp");
  libs.push_back("$LARCORE_LIB/liblarcore_Geometry");
  libs.push_back("$DUNETPC_LIB/libdune_ArtSupport");
  libs.push_back("$DUNETPC_LIB/libdune_Geometry");
  libs.push_back("$DUNETPC_LIB/libdune_DuneServiceAccess");
  libs.push_back("$DUNE_EXTENSIONS_LIB/libDXUtil");
  libs.push_back("$DUNE_EXTENSIONS_LIB/libDXGeometry");
  string libext = "so";
  string arch = gSystem->GetBuildArch();
  if ( arch.substr(0,3) == "mac" ) libext = "dylib";
  for ( int ilib=0; ilib<libs.size(); ++ilib ) {
    string lib = libs[ilib] + "." + libext;
    string libres = gSystem->ExpandPathName(lib.c_str());
    if ( 0 ) cout << "AddLinkedLibs: " << libres << endl;
    gSystem->AddLinkedLibs(libres.c_str());
  }

  gROOT->ProcessLine(".L $DUNE_EXTENSIONS_INC/DXGeometry/PlanePosition.h+");
  gROOT->ProcessLine(".L $DUNE_EXTENSIONS_INC/DXGeometry/GeoHelper.h+");

  gROOT->ProcessLine(".L palette.cxx+");
  gROOT->ProcessLine(".L gettree.cxx+");
  gROOT->ProcessLine(".L addaxis.cxx+");
  gROOT->ProcessLine(".L fix2dcanvas.cxx+");
  gROOT->ProcessLine(".L dxhist.cxx+");
  gROOT->ProcessLine(".L dxlabel.cxx+");
  gROOT->ProcessLine(".L dxopen.cxx+");
  gROOT->ProcessLine(".L dxprint.cxx+");
  gROOT->ProcessLine(".L FFTHist.cxx+");
  gROOT->ProcessLine(".L FFTHist1d.cxx+");
  gROOT->ProcessLine(".L howStuck.cxx+");
  gROOT->ProcessLine(".L TruncatedHist.cxx+");
  gROOT->ProcessLine(".L DrawResult.cxx+");
  gROOT->ProcessLine(".L draw.cxx+");
  gROOT->ProcessLine(".L draw1d.cxx+");
  gROOT->ProcessLine(".L drawpars.cxx+");
  gROOT->ProcessLine(".L getLabel.cxx+");
  gROOT->ProcessLine(".L HistoCompare.cxx+");
  gROOT->ProcessLine(".L slidingWindow.cxx+");
  gROOT->ProcessLine(".L corrHist.cxx+");
  gROOT->ProcessLine(".L PFHist.cxx+");
  gROOT->ProcessLine(".L RestrictedDFT.cxx+");
  gROOT->ProcessLine(".L RopName.cxx+");

  gROOT->ProcessLine(".L ex2dhist.C");
  //gROOT->ProcessLine(".L detlar.cxx+");
  gROOT->ProcessLine(".L drawTracks.C");

  gROOT->ProcessLine(".L $DUNETPC_INC/dune/ArtSupport/ArtServiceHelper.h+");
  gROOT->ProcessLine(".L $DUNETPC_INC/dune/ArtSupport/ArtServicePointer.h+");
  gROOT->ProcessLine(".L $DUNETPC_INC/dune/ArtSupport/ArtServiceProvider.h+");
  gROOT->ProcessLine(".L $DUNETPC_INC/dune/DuneServiceAccess/DuneServiceAccess.h+");
  gROOT->ProcessLine(".L $DUNETPC_INC/dune/DuneServiceAccess/LarsoftServiceAccess.h+");

  gStyle->SetPadRightMargin(0.14);   // For 2D plots
  gStyle->SetTitleY(0.97);

  // Read local rootlogon.
  TString myfile = "myrootlogon.C";
  if ( gSystem->FindFile(".", myfile) ) {
    gROOT->ProcessLine(".X myrootlogon.C");
  }

  //root6 drawpars::set35t();
  //root6 drawpars::set10ktw();
  //ArtServiceHelper& ash = ArtServiceHelper::instance();
  //cout << "Art service helper is ash. E.g. ash.print() to list available services." << endl;
  //root6 if ( dxopen(gApplication->InputFiles()) == 0 ) {
  //root6   cout << "Opened DXDisplay file " << gFile->GetName() << endl;
  //root6 } else {
  //root6   cout << "No DXDISPLAY input file specified." << endl;
  //root6 }

}
