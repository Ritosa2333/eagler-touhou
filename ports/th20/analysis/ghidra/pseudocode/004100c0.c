/* Entry: 0x004100c0; symbol: deallocate; body bytes: 31 */

/* Library Function - Multiple Matches With Same Base Name
    public: void __thiscall std::allocator<unsigned short>::deallocate(unsigned short *
   const,unsigned int)
    public: void __thiscall std::allocator<wchar_t>::deallocate(wchar_t * const,unsigned int)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void deallocate(LPVOID param_1,int param_2)

{
  FUN_0040cfc0(param_1,param_2 << 1);
  return;
}

