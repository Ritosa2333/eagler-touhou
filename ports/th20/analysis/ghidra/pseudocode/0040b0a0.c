/* Entry: 0x0040b0a0; symbol: _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1><class_std::_Facet_base*&>; body bytes: 35 */

/* Library Function - Single Match
    public: __thiscall std::_Compressed_pair<struct std::default_delete<class
   std::_Facet_base>,class std::_Facet_base *,1>::_Compressed_pair<struct std::default_delete<class
   std::_Facet_base>,class std::_Facet_base *,1><class std::_Facet_base * &>(struct
   std::_Zero_then_variadic_args_t,class std::_Facet_base * &)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall
std::_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
::
_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1><class_std::_Facet_base*&>
          (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           *this,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)move<>(param_3);
  *(undefined4 *)this = *puVar1;
  return this;
}

