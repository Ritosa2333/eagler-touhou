/* Entry: 0x0051cab0; symbol: setp; body bytes: 49 */

/* Library Function - Single Match
    protected: void __thiscall std::basic_streambuf<char,struct std::char_traits<char> >::setp(char
   *,char *)
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __thiscall
std::basic_streambuf<char,struct_std::char_traits<char>_>::setp
          (basic_streambuf<char,struct_std::char_traits<char>_> *this,char *param_1,char *param_2)

{
  **(undefined4 **)(this + 0x10) = param_1;
  **(undefined4 **)(this + 0x20) = param_1;
  **(int **)(this + 0x30) = (int)param_2 - (int)param_1;
  return;
}

