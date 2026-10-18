/* Entry: 0x004fcd80; symbol: Tidy; body bytes: 56 */

/* Library Function - Multiple Matches With Same Base Name
    private: void __thiscall std::numpunct<char>::_Tidy(void)
    private: void __thiscall std::numpunct<unsigned short>::_Tidy(void)
    private: void __thiscall std::numpunct<wchar_t>::_Tidy(void)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __fastcall Tidy(int param_1)

{
  thunk_FUN_00557ba0(*(LPVOID *)(param_1 + 8));
  thunk_FUN_00557ba0(*(LPVOID *)(param_1 + 0x10));
  thunk_FUN_00557ba0(*(LPVOID *)(param_1 + 0x14));
  return;
}

