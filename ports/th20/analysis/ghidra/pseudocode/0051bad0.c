/* Entry: 0x0051bad0; symbol: init; body bytes: 98 */

/* Library Function - Single Match
    protected: void __thiscall std::basic_ios<char,struct std::char_traits<char> >::init(class
   std::basic_streambuf<char,struct std::char_traits<char> > *,bool)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall
std::basic_ios<char,struct_std::char_traits<char>_>::init
          (basic_ios<char,struct_std::char_traits<char>_> *this,
          basic_streambuf<char,struct_std::char_traits<char>_> *param_1,bool param_2)

{
  basic_ios<char,struct_std::char_traits<char>_> bVar1;
  
  FUN_0051ab80(this);
  *(basic_streambuf<char,struct_std::char_traits<char>_> **)(this + 0x38) = param_1;
  *(undefined4 *)(this + 0x3c) = 0;
  bVar1 = (basic_ios<char,struct_std::char_traits<char>_>)FUN_0051ce60(this,0x20);
  this[0x40] = bVar1;
  if (*(int *)(this + 0x38) == 0) {
    setstate(this,4,false);
  }
  if (param_2) {
    ios_base::_Addstd((ios_base *)this);
  }
  return;
}

