int t0map(int run, string* pst0 =0) {
  vector<int> t0val(20000,0);
  t0val[13893] = 1456788636;
  t0val[14009] = 1456857132;
  t0val[14049] = 1456871851;
  t0val[14085] = 1456885145;
  t0val[14234] = 1456958607;
  if ( run < 0 ) return -1;
  if ( run >= t0val.size() ) return 0;
  int t0 = t0val[run];
  if ( pst0 != 0 ) {
    ostringstream sst0;
    sst0 << t0;
    *pst0 = sst0.str();
  }
  return t0;
}
