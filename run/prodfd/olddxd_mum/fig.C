void fig(string geo, int opt) {
  double tmin = 2500;
  double tmax = 2700;
  double cmin = 5200;
  double cmax = 7200;
  string clab;
  string hname = "h4_rawall";
  if ( geo == "ws" ) {
    if ( opt == 1 ) {
      tmin = 1900;
      tmax = 3700;
      clab = "all";
    } else if ( opt == 2 ) {
      cmin = 6290;
      cmax = 6370;
      clab = "ind";
    } else if ( opt == 3 ) {
      cmin = 6820;
      cmax = 6900;
      clab = "col";
    } else {
      cout << "Invalid geometry/option: " << geo << "/" << opt << endl;
      return;
    }
  } else if ( geo == "126" ) {
    hname = "h2_rawall";
    tmin = 3000;
    tmax = 3200;
    if ( opt == 1 ) {
      tmin = 2300;
      tmax = 4500;
      cmin =  5200;
      cmax =  7600;
      clab = "all";
    } else if ( opt == 2 ) {
      cmin =  6115;
      cmax =  6165;
      clab = "ind";
    } else if ( opt == 3 ) {
      cmin =  7330;
      cmax =  7380;
      clab = "col";
    } else {
      cout << "Invalid geometry/option: " << geo << "/" << opt << endl;
      return;
    }
  } else {
    cout << "Invalid geometry: " << geo << endl;
    return;
  }
  DrawResult dres = draw(hname, 0, tmin, tmax, 100);
  dres.hdraw->GetYaxis()->SetRangeUser(cmin, cmax);
  dres.hdraw->Draw("colz");
  addaxis();
  string lab;
  ifstream fin("labelprefix.txt");
  if ( fin ) fin >> lab;
  lab += dxlabel()->GetTitle();
  if ( clab.size() ) {
    lab += "_";
    lab += clab;
  }
  dxprint(lab, "", 2);
}
