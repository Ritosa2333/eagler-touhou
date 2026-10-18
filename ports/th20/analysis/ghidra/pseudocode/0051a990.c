/* Entry: 0x0051a990; symbol: _Init; body bytes: 110 */

/* Library Function - Single Match
    protected: void __thiscall std::basic_streambuf<char,struct std::char_traits<char>
   >::_Init(void)
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __thiscall
std::basic_streambuf<char,struct_std::char_traits<char>_>::_Init
          (basic_streambuf<char,struct_std::char_traits<char>_> *this)

{
  *(basic_streambuf<char,struct_std::char_traits<char>_> **)(this + 0xc) = this + 4;
  *(basic_streambuf<char,struct_std::char_traits<char>_> **)(this + 0x10) = this + 8;
  *(basic_streambuf<char,struct_std::char_traits<char>_> **)(this + 0x1c) = this + 0x14;
  *(basic_streambuf<char,struct_std::char_traits<char>_> **)(this + 0x20) = this + 0x18;
  *(basic_streambuf<char,struct_std::char_traits<char>_> **)(this + 0x2c) = this + 0x24;
  *(basic_streambuf<char,struct_std::char_traits<char>_> **)(this + 0x30) = this + 0x28;
  setp(this,(char *)0x0,(char *)0x0);
  setg(this,(char *)0x0,(char *)0x0,(char *)0x0);
  return;
}

