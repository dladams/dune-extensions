void trigtime(int run, float tmax =160) {
  ostingstream ssrun;
  ssrun << run;
  string srun = srun;
  ssfname << "run" << run << "evt.root";
  string fname = "run" + srun + "evt.root";
  TFile* pfile = new TFile(fname.c_str(), "READ");
  if ( pfile == 0 || !pfile->IsOpen()) {
    cout << "File not found: " << fname << endl;
    return;
  }
  TTree* ptree = dynamic_cast<TTree*>(pfile->Get("DXDisplay/EventTree"));
  if ( ptree == 0 ) {
    cout << "Tree not found." << endl;
    return;
  }
  new TCanvas;
  string stit = "Trigger times; Time [sec]; Trigger (109 is ghost)";
  string sarg = "trigger-(trigger>100)*109+109:tlo-1456885145";
  string sarg = "trigger-(trigger>100)*109+109:tlo-1456885145";
  TH2* ph2 = new TH2F("h2", stit.c_str(), 100, 0, tmax, 8, 107.5, 116.5);
  ph2->SetStats(0);
  ph2->Draw();
  ptree->SetMarkerStyle(2);
  ptree->Draw(sarg.c_str(), "", "same");
}
  
