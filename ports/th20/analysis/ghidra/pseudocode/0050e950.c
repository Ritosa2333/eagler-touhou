/* Entry: 0x0050e950; symbol: ~unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>; body bytes: 53 */

/* Library Function - Single Match
    public: __thiscall std::unique_ptr<class std::_Facet_base,struct std::default_delete<class
   std::_Facet_base> >::~unique_ptr<class std::_Facet_base,struct std::default_delete<class
   std::_Facet_base> >(void)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall
std::unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>::
~unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
          (unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
           *this)

{
  if (*(int *)this != 0) {
    _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
    _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)this);
    FUN_0050eaf0(*(LPVOID *)this);
  }
  return;
}

