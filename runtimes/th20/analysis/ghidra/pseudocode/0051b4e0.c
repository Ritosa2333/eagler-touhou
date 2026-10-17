/* Entry: 0x0051b4e0; symbol: egptr; body bytes: 29 */

/* Library Function - Single Match
    protected: char * __thiscall std::basic_streambuf<char,struct std::char_traits<char>
   >::egptr(void)const 
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

char * __thiscall
std::basic_streambuf<char,struct_std::char_traits<char>_>::egptr
          (basic_streambuf<char,struct_std::char_traits<char>_> *this)

{
  return (char *)(**(int **)(this + 0x1c) + **(int **)(this + 0x2c));
}

