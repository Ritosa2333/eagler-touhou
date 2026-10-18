/* Entry: 0x0051b850; symbol: gbump; body bytes: 51 */

/* Library Function - Single Match
    protected: void __thiscall std::basic_streambuf<char,struct std::char_traits<char> >::gbump(int)
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __thiscall
std::basic_streambuf<char,struct_std::char_traits<char>_>::gbump
          (basic_streambuf<char,struct_std::char_traits<char>_> *this,int param_1)

{
  **(int **)(this + 0x2c) = **(int **)(this + 0x2c) - param_1;
  **(int **)(this + 0x1c) = **(int **)(this + 0x1c) + param_1;
  return;
}

