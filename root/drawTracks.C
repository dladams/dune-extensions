#include "drawpars.h"
#include "gettree.h"

/*
void add35tTpcs() {
  const int ntpc = 8;
  float xmin[ntpc] = {-34.45, -0.75, -34.45, -0.75, -34.45, 0.75, -34.45, 0.75};
  float xmax[ntpc] = {-7.28, 221.73, -7.28, 221.73, -7.28, 221.73, -7.28, 221.73};
  float ymin[ntpc] = {-79.94, -79.94, -89.05, -89.05, 3.83, 3.83, -79.94, -79.94};
  float ymax[ntpc] = {116.97, 116.97, -3.83, -3.83, 116.97, 116.97, 116.97, 116.97};
  float zmin[ntpc] = {-2.04, -2.04, 51.41, 51.41, 51.41, 51.41, 103.33, 103.33};
  float zmax[ntpc] = {51.41, 51.41, 103.33, 103.33, 103.33, 103.33, 156.78, 156.78};
}
*/

TCanvas* drawTracks(string sel ="event==1", string var ="pty:ptz:ptx", string hvar="") {
  int dbg = 1;
  if ( mcptree() == 0 ) return 0;
  ostringstream ssbasesel;
  ssbasesel << "ptx>" << drawpars::xmin << "&&ptx<" << drawpars::xmax;
  ssbasesel << "&&pty>" << drawpars::ymin << "&&pty<" << drawpars::ymax;
  ssbasesel << "&&ptz>" << drawpars::zmin << "&&ptz<" << drawpars::zmax;
  string basesel = ssbasesel.str();
  if ( sel != "" ) basesel += "&&" + sel;
  string sel0 = basesel + "&&rpdg>=5";          // neutrals
  string sel1 = basesel + "&&rpdg<5&&pttpc<0";  // charged outside detector volume
  string sel2 = basesel + "&&rpdg<5&&pttpc>=0"; // charged in detector volume
  cout << "Base selection: " << sel0 << endl;
  int col0 = 4;
  int col1 = 3;
  int col2 = 2;
  int oldstyle = mcptree()->GetMarkerStyle();
  int oldcolor = mcptree()->GetMarkerColor();
  int oldlinecolor = mcptree()->GetLineColor();
  string var0 = var;
  if ( hvar != "" ) var0 += ">>" + hvar;
  TCanvas* pcan = new TCanvas;
  mcptree()->SetMarkerStyle(1);
  string sopt;
  // Draw axis.
  if ( var == "pty:ptz:ptx" ) {
    TH3F* paxis = new TH3F("hax", sel.c_str(), 20, drawpars::xmin, drawpars::xmax,
                                               20, drawpars::zmin, drawpars::zmax,
                                               20, drawpars::ymin, drawpars::ymax);
    paxis->SetStats(0);
    paxis->GetXaxis()->SetTitle("x [cm]");
    paxis->GetXaxis()->SetTitleOffset(1.6);
    paxis->GetYaxis()->SetTitle("z [cm]");
    paxis->GetYaxis()->SetTitleOffset(1.6);
    paxis->GetZaxis()->SetTitle("y [cm]");
    paxis->GetZaxis()->SetTitleOffset(1.3);
    if ( dbg ) cout << "Drawing axis." << endl;
    paxis->Draw();
    sopt = "same";
  }
  // Draw 0
  mcptree()->SetMarkerColor(col0);
  mcptree()->SetLineColor(col0);
  if ( dbg ) cout << "Drawing neutrals." << endl;
  unsigned int nsel0 = mcptree()->Draw(var0.c_str(), sel0.c_str(), sopt.c_str());
  // Draw 1
  mcptree()->SetMarkerColor(col1);
  mcptree()->SetLineColor(col1);
  if ( nsel0 > 0 ) sopt = "same";
  if ( dbg ) cout << "Drawing charged outside TPCs." << endl;
  unsigned int nsel1 = mcptree()->Draw(var.c_str(), sel1.c_str(), sopt.c_str());
  // Draw 2
  mcptree()->SetMarkerColor(col2);
  mcptree()->SetLineColor(col2);
  if ( nsel1 > 0 ) sopt = "same";
  if ( dbg ) cout << "Drawing charged inside TPCs." << endl;
  unsigned int nsel2 = mcptree()->Draw(var.c_str(), sel2.c_str(), sopt.c_str());
  // Closeout
  mcptree()->SetMarkerStyle(oldstyle);
  mcptree()->SetMarkerColor(oldcolor);
  mcptree()->SetLineColor(oldlinecolor);
  cout << "Selection: " << basesel << endl;
  cout << "# selected: " <<nsel0 << ", " << nsel1 << ", " << nsel2 << endl;
  pcan->Update();
  // Scan 1.
  string sctitle = "                            event       track     process         PDG";
  sctitle += "         TPC             position [cm]             time [ms]     E [MeV]";
  if ( 0 ) {
    cout << sctitle << endl;
    string scvar1 = "event:trackid:proc:pdg:pttpc[0]:ptx[0]:pty[0]:ptz[0]:int(ptt[0]*0.001):int(1000*pte[0])";
    mcptree()->Scan(scvar1.c_str(), basesel.c_str());
    cout << sctitle << endl;
  }
  // Scan 2.
  cout << sctitle << endl;
  string scvar = "event:trackid:proc:pdg:pttpc:ptx:pty:ptz:int(ptt*0.001):int(1000*pte)";
  mcptree()->Scan(scvar.c_str(), basesel.c_str());
  cout << sctitle << endl;
  // Done.
  return pcan;
}
