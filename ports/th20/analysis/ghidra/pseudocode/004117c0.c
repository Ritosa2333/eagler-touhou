/* Entry: 0x004117c0; symbol: what; body bytes: 43 */

/* Library Function - Single Match
    public: virtual char const * __thiscall std::exception::what(void)const 
   
   Library: Visual Studio */

char * __thiscall std::exception::what(exception *this)

{
  char *local_c;
  
  if (*(int *)(this + 4) == 0) {
    local_c = "Unknown exception";
  }
  else {
    local_c = *(char **)(this + 4);
  }
  return local_c;
}

