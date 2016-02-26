void ped() {
  gStyle->SetPadRightMargin(0.02);
  event1->cd();
  TLine* pl1 = new TLine(0,0,1024,0);
  TLine* pl2 = new TLine(1024, 0,2048,0);
  TCanvas* pcan = new TCanvas;
  pcan->Divide(1,2);
  TH1* ph0 = h1_rawmean;
  ph0->SetMaximum(100);
  ph0->SetMinimum(-100);
  ph0->SetStats(0);
  pcan->cd(1);
  TH1* ph1 = dynamic_cast<TH1*>(ph0->Clone("hped1"));
  ph1->GetXaxis()->SetRangeUser(0,1024);
  ph1->Draw();
  pl1->Draw();
  pcan->cd(2);
  TH1* ph2 = dynamic_cast<TH1*>(ph0->Clone("hped1"));
  ph2->GetXaxis()->SetRangeUser(1024, 2048);
  ph2->Draw();
  pl2->Draw();
  ph0->GetXaxis()->SetRangeUser(0, 2048);
  pcan->cd(0);
}

