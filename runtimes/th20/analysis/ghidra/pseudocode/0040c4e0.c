/* Entry: 0x0040c4e0; symbol: release; body bytes: 36 */

/* Library Function - Single Match
    public: class std::_Facet_base * __thiscall std::unique_ptr<class std::_Facet_base,struct
   std::default_delete<class std::_Facet_base> >::release(void)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

_Facet_base * __thiscall
std::unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>::release
          (unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
           *this)

{
  _Facet_base *p_Var1;
  _Facet_base *local_8;
  
  local_8 = (_Facet_base *)0x0;
  p_Var1 = exchange<class_std::_Facet_base*,class_std::_Facet_base*>((_Facet_base **)this,&local_8);
  return p_Var1;
}

