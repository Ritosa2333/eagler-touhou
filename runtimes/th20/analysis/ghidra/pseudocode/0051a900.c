/* Entry: 0x0051a900; symbol: _Gninc; body bytes: 65 */

/* Library Function - Single Match
    protected: char * __thiscall std::basic_streambuf<char,struct std::char_traits<char>
   >::_Gninc(void)
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

char * __thiscall
std::basic_streambuf<char,struct_std::char_traits<char>_>::_Gninc
          (basic_streambuf<char,struct_std::char_traits<char>_> *this)

{
  char *pcVar1;
  
  **(int **)(this + 0x2c) = **(int **)(this + 0x2c) + -1;
  pcVar1 = (char *)**(undefined4 **)(this + 0x1c);
  **(int **)(this + 0x1c) = **(int **)(this + 0x1c) + 1;
  return pcVar1;
}

