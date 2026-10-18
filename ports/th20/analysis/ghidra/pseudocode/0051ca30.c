/* Entry: 0x0051ca30; symbol: setg; body bytes: 49 */

/* Library Function - Single Match
    protected: void __thiscall std::basic_streambuf<char,struct std::char_traits<char> >::setg(char
   *,char *,char *)
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __thiscall
std::basic_streambuf<char,struct_std::char_traits<char>_>::setg
          (basic_streambuf<char,struct_std::char_traits<char>_> *this,char *param_1,char *param_2,
          char *param_3)

{
  **(undefined4 **)(this + 0xc) = param_1;
  **(undefined4 **)(this + 0x1c) = param_2;
  **(int **)(this + 0x2c) = (int)param_3 - (int)param_2;
  return;
}

