/* Entry: 0x0051caf0; symbol: setstate; body bytes: 39 */

/* Library Function - Single Match
    public: void __thiscall std::basic_ios<char,struct std::char_traits<char> >::setstate(int,bool)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __thiscall
std::basic_ios<char,struct_std::char_traits<char>_>::setstate
          (basic_ios<char,struct_std::char_traits<char>_> *this,int param_1,bool param_2)

{
  uint uVar1;
  
  uVar1 = FUN_0041cab0((int)this);
  FUN_0051b1e0(this,uVar1 | param_1,param_2);
  return;
}

