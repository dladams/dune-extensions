void plot_rcecorr(TH2* phin, int rce1=0, int rce2=0) {
  double zmax = 100.0;
  int rcefac = 128;
  for ( int rce=rce1; rce<=rce2; ++rce ) {
    ostringstream ssnam;
    ssnam << "h" << ievt << "_rawallon";
      cout << "Drawing e" << rce << endl;
      ssnam << "_rce" << rce;
      double ymin = rcefac*rce;
      double ymax = ymin + rcefac;
      TH2* ph = dynamic_cast<TH2*>(res.hdraw->Clone(ssnam.str().c_str()));
      ph->GetYaxis()->SetRangeUser(ymin, ymax);
      ostringstream sstit;
      sstit << ph->GetTitle() << " RCE " << rce;
      ph->SetTitle(sstit.str().c_str());
      ph->Draw("colz");
      addaxis();
      ostringstream ssfnam;
      ssfnam << dxlabel()->GetTitle() << "_event" << ievt << "_rce" << rce << "_tick" << xmin;
      dxprint(ssfnam.str(), "", 2);
    }
  }
}
