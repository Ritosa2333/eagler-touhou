/* Entry: 0x0052d250; symbol: _Delete_plain_internal<class_std::allocator<struct_std::_Container_proxy>_>; body bytes: 21 */

/* Library Function - Single Match
    void __cdecl std::_Delete_plain_internal<class std::allocator<struct std::_Container_proxy>
   >(class std::allocator<struct std::_Container_proxy> &,struct std::_Container_proxy * const)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __cdecl
std::_Delete_plain_internal<class_std::allocator<struct_std::_Container_proxy>_>
          (allocator<struct_std::_Container_proxy> *param_1,_Container_proxy *param_2)

{
  _Deallocate_plain<class_std::allocator<struct_std::_Container_proxy>_>(param_1,param_2);
  return;
}

