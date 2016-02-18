void plot_online(int evt1 =1, int evt2 =0) {
  if ( evt2 < evt1 ) evt2 = evt1;
  double xmin = 0.0;
  double xmax = 0.0;
  double zmax = 100.0;
  for ( int ievt=evt1; ievt<=evt2; ++ievt ) {
    ostringstream ssnam;
    ssnam << "h" << ievt << "_rawallon";
    draw(ssnam.str(), 0, xmin, xmax, zmax);
    dxprint("", "", 2);
  }
}
