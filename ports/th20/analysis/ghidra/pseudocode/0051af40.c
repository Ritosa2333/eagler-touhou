/* Entry: 0x0051af40; symbol: _Pninc; body bytes: 65 */

/* Library Function - Multiple Matches With Different Base Names
    protected: char * __thiscall std::basic_streambuf<char,struct std::char_traits<char>
   >::_Gninc(void)
    protected: char * __thiscall std::basic_streambuf<char,struct std::char_traits<char>
   >::_Pninc(void)
   
   Library: Visual Studio */

char * __thiscall
std::basic_streambuf<char,struct_std::char_traits<char>_>::_Pninc
          (basic_streambuf<char,struct_std::char_traits<char>_> *this)

{
  char *pcVar1;
  
  **(int **)(this + 0x30) = **(int **)(this + 0x30) + -1;
  pcVar1 = (char *)**(undefined4 **)(this + 0x20);
  **(int **)(this + 0x20) = **(int **)(this + 0x20) + 1;
  return pcVar1;
}

