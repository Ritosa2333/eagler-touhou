/* Entry: 0x0052d1b0; symbol: _Alloc_proxy<class_std::allocator<struct_std::_Container_proxy>_>; body bytes: 75 */

/* Library Function - Single Match
    public: void __thiscall std::_Container_base12::_Alloc_proxy<class std::allocator<struct
   std::_Container_proxy> >(class std::allocator<struct std::_Container_proxy> &&)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall
std::_Container_base12::_Alloc_proxy<class_std::allocator<struct_std::_Container_proxy>_>
          (_Container_base12 *this,allocator<struct_std::_Container_proxy> *param_1)

{
  undefined4 uVar1;
  _Container_base12 *local_10;
  _Container_base12 *local_c;
  undefined4 *local_8;
  
  local_c = this;
  uVar1 = FUN_00415030(param_1,1);
  local_8 = (undefined4 *)move<>(uVar1);
  local_10 = local_c;
  FUN_004ad770(local_8,&local_10);
  *(undefined4 **)local_c = local_8;
  *local_8 = local_c;
  return;
}

