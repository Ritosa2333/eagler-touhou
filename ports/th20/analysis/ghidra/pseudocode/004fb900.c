/* Entry: 0x004fb900; symbol: c_str; body bytes: 51 */

/* Library Function - Single Match
    public: char const * __thiscall std::locale::c_str(void)const 
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

char * __thiscall std::locale::c_str(locale *this)

{
  char *local_c;
  
  if (*(int *)(this + 4) == 0) {
    local_c = "";
  }
  else {
    local_c = _Yarn<char>::c_str((_Yarn<char> *)(*(int *)(this + 4) + 0x18));
  }
  return local_c;
}

