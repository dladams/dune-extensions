// fig001.C
//
// David Adams
// October 2015
//
// Root macro to draw rawm deconvoted and SimChannel signals as
// function of TDC tick for one channel.
//
//   itrk = track number, e.g. different positions in TPC.

void fig001(int itrk=1) {
  TCanvas* pcan = new TCanvas;
  gStyle->SetHistLineWidth(2);
  gStyle->SetOptStat(0);
  gStyle->SetTitleY(0.97);
  draw("h1_dcoapa0z2");
  draw("h1_rawapa0z2");
  if ( itrk == 1 ) draw("h1_mcs001apa0z2");
  if ( itrk == 2 ) draw("h1_mcs002apa0z2");
  if ( itrk == 3 ) draw("h1_mcs003apa0z2");
  TH1* phr = hdraw2x070;
  TH1* phd = hdraw1x070;
  TH1* phs = hdraw3x070;
  if ( itrk == 1 ) phr->GetXaxis()->SetRangeUser( 240,  290);
  if ( itrk == 2 ) phr->GetXaxis()->SetRangeUser(1360, 1410);
  if ( itrk == 3 ) phr->GetXaxis()->SetRangeUser(2480, 2530);
  phr->SetLineColor(38);
  phd->SetLineColor(30);
  phs->SetLineColor(46);
  phd->SetLineStyle(2);
  phs->SetLineStyle(3);
  phs->Scale(40);
  phr->SetMinimum(-5);
  TCanvas* pcan = new TCanvas;
  phr->Draw();
  phd->Draw("same");
  phs->Draw("same");
  TLegend* pleg = new TLegend(0.12, 0.70, 0.45, 0.88);
  pleg->AddEntry(phr, "Raw", "l");
  pleg->AddEntry(phd, "Deconvoluted", "l");
  pleg->AddEntry(phs, "SimChannel x40", "l");
  pleg->SetBorderSize(0);
  pleg->Draw();
  ostringstream sstrk;
  sstrk << itrk;
  string fname = "fig001-";
  fname += sstrk.str().c_str();
  fname += ".png";
  pcan->Print(fname.c_str());
  cout << "Output file is " << fname << endl;
}
