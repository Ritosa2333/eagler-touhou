/* Entry: 0x004fed70; symbol: c_str; body bytes: 43 */

/* Library Function - Single Match
    public: char const * __thiscall std::_Yarn<char>::c_str(void)const 
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

char * __thiscall std::_Yarn<char>::c_str(_Yarn<char> *this)

{
  _Yarn<char> *local_c;
  
  if (*(int *)this == 0) {
    local_c = this + 4;
  }
  else {
    local_c = *(_Yarn<char> **)this;
  }
  return (char *)local_c;
}

