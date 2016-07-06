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
  cout << "Welcome to DXRoot." << endl;

  string mpath = TROOT::GetMacroPath();
  mpath += "$DUNE_EXTENSIONS_ROOTSCRIPTS:";
  TROOT::SetMacroPath(mpath.c_str());
  gSystem->AddIncludePath("-I$DUNE_EXTENSIONS_ROOTSCRIPTS");
  gSystem->SetBuildDir(".aclic");

  gSystem->AddIncludePath("-I$BOOST_INC");
  //gSystem->AddIncludePath("-DBOOST_NO_CWCHAR");
  gSystem->AddIncludePath("-I$CPP0X_INC");
  gSystem->AddIncludePath("-I$CETLIB_INC");
  gSystem->AddIncludePath("-I$FHICLCPP_INC");
  gSystem->AddIncludePath("-I$CLHEP_INC");
  gSystem->AddIncludePath("-I$LARCORE_INC");
  gSystem->AddIncludePath("-I$LAREVT_INC");
  gSystem->AddIncludePath("-I$DUNETPC_INC");
  gSystem->AddIncludePath("-I$ART_EXTENSIONS_INC");
  gSystem->AddIncludePath("-I$ART_INC");
  gSystem->AddIncludePath("-I$DUNE_EXTENSIONS_INC");

  gSystem->AddDynamicPath("-L$FHICLCPP_LIB -lfhiclcpp");

  // Add linked libs.
  vector<string> libs;
  libs.push_back("$CETLIB_LIB/libcetlib");
  libs.push_back("$FHICLCPP_LIB/libfhiclcpp");
  libs.push_back("$LARCORE_LIB/liblarcore_Geometry");
  libs.push_back("$DUNETPC_LIB/libdune_ArtSupport");
  libs.push_back("$DUNETPC_LIB/libdune_Geometry");
  libs.push_back("$DUNE_EXTENSIONS_LIB/libDXUtil");
  libs.push_back("$DUNE_EXTENSIONS_LIB/libDXGeometry");
  string libext = "so";
  string arch = gSystem->GetBuildArch();
  if ( arch.substr(0,3) == "mac" ) libext = "dylib";
  for ( int ilib=0; ilib<libs.size(); ++ilib ) {
    string lib = libs[ilib] + "." + libext;
    gSystem->AddLinkedLibs(lib.c_str());
  }

  gROOT->ProcessLine(".L $DUNE_EXTENSIONS_INC/DXGeometry/PlanePosition.h+");
  gROOT->ProcessLine(".L $DUNE_EXTENSIONS_INC/DXGeometry/GeoHelper.h+");

  gROOT->ProcessLine(".L palette.cxx+");
  gROOT->ProcessLine(".L gettree.cxx+");
  gROOT->ProcessLine(".L addaxis.cxx+");
  gROOT->ProcessLine(".L fix2dcanvas.cxx+");
  gROOT->ProcessLine(".L dxhist.cxx+");
  gROOT->ProcessLine(".L dxlabel.cxx+");
  gROOT->ProcessLine(".L dxprint.cxx+");
  gROOT->ProcessLine(".L DrawResult.cxx+");
  gROOT->ProcessLine(".L draw.cxx+");
  gROOT->ProcessLine(".L draw1d.cxx+");
  gROOT->ProcessLine(".L drawpars.cxx+");
  gROOT->ProcessLine(".L getLabel.cxx+");
  gROOT->ProcessLine(".L HistoCompare.cxx+");
  gROOT->ProcessLine(".L slidingWindow.cxx+");
  gROOT->ProcessLine(".L corrHist.cxx+");
  gROOT->ProcessLine(".L FFTHist.cxx+");
  gROOT->ProcessLine(".L PFHist.cxx+");

  draw::set35t();
  draw::set10ktw();

  gROOT->ProcessLine(".L ex2dhist.C");
  //gROOT->ProcessLine(".L detlar.cxx+");
  gROOT->ProcessLine(".L drawTracks.C");
  gROOT->ProcessLine(".L dxopen.C");

  gROOT->ProcessLine(".L $DUNETPC_INC/dune/ArtSupport/aclic_ArtServiceHelper.h+");
  ArtServiceHelper& ash = ArtServiceHelper::instance();
  cout << "Art service helper is ash. E.g. ash.print() to list available services.";


  gStyle->SetPadRightMargin(0.14);   // For 2D plots
  gStyle->SetTitleY(0.97);

  // Read local rootlogon.
  if ( gSystem->FindFile(".", "myrootlogon.C") ) {
    gROOT->ProcessLine(".X myrootlogon.C");
  }

  if ( dxopen(gApplication->InputFiles()) == 0 ) {
    cout << "Opened DXDisplay file " << gFile->GetName() << endl;
  } else {
    cout << "No DXDISPLAY input file specified." << endl;
  }

}
