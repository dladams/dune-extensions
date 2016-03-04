int dsindex(int run) {
  if ( run < 13370 ) return -1;
  if ( run < 14008 ) return 0;
  if ( run < 14074 ) return 1;
  if ( run < 14600 ) return 2;
  return -2;
}
