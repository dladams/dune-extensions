void plot_corr(TH2* phin, float cmin, float cmax) {
  ostringstream ssnam;
  ssnam << phin->GetName() << "_" << cmin << "_" << cmax;
  new TCanvas;
  TH2* ph = phin->Clone(ssnam.str().c_str());
  ph->GetXaxis()->SetRangeUser(cmin, cmax);
  ph->GetYaxis()->SetRangeUser(cmin, cmax);
  ph->Draw("colz");
  addaxis();
}
