/* Entry: 0x0051b040; symbol: _Tidy; body bytes: 84 */

/* Library Function - Single Match
    protected: void __thiscall std::ctype<char>::_Tidy(void)
   
   Libraries: Visual Studio 2012 Debug, Visual Studio 2012 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __thiscall std::ctype<char>::_Tidy(ctype<char> *this)

{
  if (*(int *)(this + 0x10) < 1) {
    if (*(int *)(this + 0x10) < 0) {
      thunk_FUN_00557ba0(*(LPVOID *)(this + 0xc));
    }
  }
  else {
    thunk_FUN_00557ba0(*(LPVOID *)(this + 0xc));
  }
  thunk_FUN_00557ba0(*(LPVOID *)(this + 0x14));
  return;
}

