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
    tmin = 2200;
    tmax = 2400;
    if ( opt == 1 ) {
      tmin = 1800;
      tmax = 2700;
      cmin =  7900;
      cmax = 10300;
      clab = "all";
    } else if ( opt == 2 ) {
      cmin =  8600;
      cmax =  8650;
      clab = "ind";
    } else if ( opt == 3 ) {
      cmin = 10100;
      cmax = 10200;
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
