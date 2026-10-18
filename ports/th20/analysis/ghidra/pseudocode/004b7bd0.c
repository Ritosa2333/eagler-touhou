/* Entry: 0x004b7bd0; symbol: allocate; body bytes: 34 */

/* Library Function - Single Match
    public: struct std::_Container_proxy * __thiscall std::allocator<struct
   std::_Container_proxy>::allocate(unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

_Container_proxy * __thiscall
std::allocator<struct_std::_Container_proxy>::allocate
          (allocator<struct_std::_Container_proxy> *this,uint param_1)

{
  uint uVar1;
  _Container_proxy *p_Var2;
  
  uVar1 = _Get_size_of_n<8>(param_1);
  p_Var2 = (_Container_proxy *)FUN_0040c770(uVar1);
  return p_Var2;
}

