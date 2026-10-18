/* Entry: 0x0052d200; symbol: _Deallocate_plain<class_std::allocator<struct_std::_Container_proxy>_>; body bytes: 67 */

/* Library Function - Single Match
    void __cdecl std::_Deallocate_plain<class std::allocator<struct std::_Container_proxy> >(class
   std::allocator<struct std::_Container_proxy> &,struct std::_Container_proxy * const)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __cdecl
std::_Deallocate_plain<class_std::allocator<struct_std::_Container_proxy>_>
          (allocator<struct_std::_Container_proxy> *param_1,_Container_proxy *param_2)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005676c0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0052f040(param_1,param_2,1);
  *unaff_FS_OFFSET = local_10;
  return;
}

