void plot_rce(int rce1=0, int rce2=0, int evt1=1, int evt2=0,
              double xmin =3000, double xmax =-10000) {
  if ( evt2 < evt1 ) evt2 = evt1;
  if ( xmax < xmin ) xmax = xmin + 500;
  double zmax = 100.0;
  int rcefac = 128;
  for ( int ievt=evt1; ievt<=evt2; ++ievt ) {
    ostringstream ssnam;
    ssnam << "h" << ievt << "_rawallon";
    DrawResult res = draw(ssnam.str(), 0, xmin, xmax, zmax);
    for ( int rce=rce1; rce<=rce2; ++rce ) {
      cout << "Drawing event " << ievt << " RCE " << rce << endl;
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
