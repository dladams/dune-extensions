void evtime() {
  vector<int> runs;
  vector<int> marks;
  vector<int> mcols;
  runs.push_back(13893);
  marks.push_back(2);
  mcols.push_back(2);
  runs.push_back(14009);
  marks.push_back(24);
  mcols.push_back(4);
  runs.push_back(14085);
  marks.push_back(25);
  mcols.push_back(3);
  runs.push_back(14234);
  marks.push_back(5);
  mcols.push_back(3);
  runs.push_back(14434);
  marks.push_back(28);
  mcols.push_back(3);
  string pre = "run";
  string suf = "evt.root";
  double tmax = 160;
  double emax = 180;
  new TCanvas;
  TH1* ph = new TH2F("hevtime", "Event vs. time; Time [sec]; Event",
                     tmax, 0, tmax, emax, 0, emax);
  ph->SetStats(0);
  ph->Draw("axis");
  TLegend* pleg = new TLegend(0.65,0.15,0.85,0.37);
  pleg->SetBorderSize(0);
  for ( unsigned int isam=0; isam<runs.size(); ++isam ) {
    int run = runs[isam];
    int icol = dsindex(run);
    int col = colormap(icol);
    ostringstream ssrun;
    ssrun << run;
    string srun = ssrun.str();
    ostringstream sst0;
    sst0 << t0map(run);
    string fname = pre + srun + suf;
    TFile* pfile = TFile::Open(fname.c_str(), "READ");
    TTree* ptree = dynamic_cast<TTree*>(pfile->Get("DXDisplay/EventTree"));
    ptree->SetMarkerStyle(marks[isam]);
    ptree->SetMarkerColor(col);
cout << run << " " << col << " " << sst0.str() <<  endl;
    if ( ptree == 0 ) {
      cout << "Tree not found" << endl;
      pfile->ls();
      return;
    }
    string sarg = "event:tlo-";
    sarg += sst0.str();
    sarg += ">>hevtime";
    ptree->Draw(sarg.c_str(), "", "same");
    pleg->AddEntry(ptree, srun.c_str(), "p");
  }
  pleg->Draw();
}
