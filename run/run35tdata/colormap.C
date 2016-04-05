int colormap(int idx) {
  vector<int> cols;
  cols.push_back(38);
  cols.push_back(46);
  cols.push_back(30);
  cols.push_back(28);
  cols.push_back( 9);
  cols.push_back( 8);
  cols.push_back(44);
  int icol = idx % cols.size();
  return cols[icol];
}
