/* Entry: 0x0040fb40; symbol: allocate; body bytes: 34 */

/* Library Function - Multiple Matches With Same Base Name
    public: unsigned short * __thiscall std::allocator<unsigned short>::allocate(unsigned int)
    public: wchar_t * __thiscall std::allocator<wchar_t>::allocate(unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void allocate(uint param_1)

{
  uint uVar1;
  
  uVar1 = std::_Get_size_of_n<2>(param_1);
  FUN_0040c770(uVar1);
  return;
}

