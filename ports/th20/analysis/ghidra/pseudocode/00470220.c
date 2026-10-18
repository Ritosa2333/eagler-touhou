/* Entry: 0x00470220; symbol: allocate; body bytes: 34 */

/* Library Function - Multiple Matches With Same Base Name
    public: char * __thiscall std::allocator<char>::allocate(unsigned int)
    public: unsigned short * __thiscall std::allocator<unsigned short>::allocate(unsigned int)
    public: struct std::_Container_proxy * __thiscall std::allocator<struct
   std::_Container_proxy>::allocate(unsigned int)
    public: wchar_t * __thiscall std::allocator<wchar_t>::allocate(unsigned int)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void allocate(uint param_1)

{
  uint uVar1;
  
  uVar1 = FUN_00413730(param_1);
  FUN_0040c770(uVar1);
  return;
}

