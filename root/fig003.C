// fig001.C
//
// David Adams
// October 2015
//
// Root macro to draw histogram showing charge sharing between channels.
//
//   itrk = track number, e.g. different positions in TPC.

void fig003(string sopt ="help") {
  TCanvas* pcan = new TCanvas;
  gStyle->SetHistLineWidth(2);
  gStyle->SetOptStat(0);
  gStyle->SetTitleY(0.97);
  string lab;
  if ( sopt == "high" || sopt == "low" ) {
    int chmin = 3800;
    int chmax = 3850;
    if ( sopt == "low" ) {
      chmin = 200;
      chmax = 250;
    }
    draw("h4_dcoapa1z2",    0, chmin, chmax);
    draw("h4_rawapa1z2",    0, chmin, chmax);
    draw("h4_mcs001apa1z2", 0, chmin, chmax);
    TH1* phr = hdraw2y;
    TH1* phd = hdraw1y;
    TH1* phs = hdraw3y;
    phr->GetXaxis()->SetRangeUser(200, 230);
    phr->GetYaxis()->SetRangeUser(0.1, 10000);
    phr->SetLineColor(38);
    phd->SetLineColor(30);
    phs->SetLineColor(46);
    phd->SetLineStyle(2);
    phs->SetLineStyle(3);
    phs->Scale(40);
    pcan = new TCanvas;
    string stit1 = phr->GetTitle();
    string stit2 = "S" + stit1.substr(5);
    phr->SetTitle(stit2.c_str());
    phr->Draw();
    phd->Draw("same");
    phs->Draw("same");
    TLegend* pleg = new TLegend(0.52, 0.70, 0.85, 0.88);
    pleg->AddEntry(phr, "Raw", "l");
    pleg->AddEntry(phd, "Deconvoluted", "l");
    pleg->AddEntry(phs, "SimChannel x40", "l");
    pleg->SetBorderSize(0);
    pleg->Draw();
    pcan->SetLogy();
    lab = "all";
  } else if ( sopt == "raw" ) {
    draw("h4_rawapa1z2");
    TH2* ph = hdraw1;
    ph->SetMaximum(20);
    ph->SetMinimum(-20);
    ph->GetYaxis()->SetRangeUser(200,230);
    pcan = new TCanvas;
    ph->Draw("colz");
    lab = "raw";
  } else if ( sopt == "mcs" ) {
    draw("h4_mcs001apa1z2");
    TH2* ph = hdraw1;
    ph->SetMaximum(0.1);
    ph->GetYaxis()->SetRangeUser(200,230);
    pcan = new TCanvas;
    ph->Draw("colz");
    lab = "mcs";
  } else if ( sopt == "dco" ) {
    draw("h4_dcoapa1z2");
    TH2* ph = hdraw1;
    ph->SetMaximum(10);
    ph->GetYaxis()->SetRangeUser(200,230);
    pcan = new TCanvas;
    ph->Draw("colz");
    lab = "dco";
  } else {
    cout << "Usage: root.exe 'fig003.C(\"SOPT\")'" << endl;
    cout << "  SOPT = low, high, raw, mcs or dco" << endl;
    return;
  }
  string fname = "fig003-";
  fname += sopt;
  fname += ".png";
  pcan->Print(fname.c_str());
  cout << "Output file is " << fname << endl;
}
