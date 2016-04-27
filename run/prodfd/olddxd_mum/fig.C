void fig(string geo, int opt) {
  double tmin = 2500;
  double tmax = 2700;
  double cmin = 5200;
  double cmax = 7200;
  string clab;
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
    tmin = 3530;
    tmax = 3730;
    if ( opt == 1 ) {
      tmin = 3400;
      tmax = 3900;
      cmin = 10100;
      cmax = 12700;
      clab = "all";
    } else if ( opt == 2 ) {
      cmin = 11100;
      cmax = 11300;
      clab = "ind";
    } else if ( opt == 3 ) {
      cmin = 12420;
      cmax = 12540;
      clab = "col";
    } else {
      cout << "Invalid geometry/option: " << geo << "/" << opt << endl;
      return;
    }
  } else {
    cout << "Invalid geometry: " << geo << endl;
    return;
  }
  DrawResult dres = draw("h4_rawall", 0, tmin, tmax, 100);
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
