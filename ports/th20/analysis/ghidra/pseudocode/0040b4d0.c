/* Entry: 0x0040b4d0; symbol: exchange<class_std::_Facet_base*,class_std::_Facet_base*>; body bytes: 29 */

/* Library Function - Single Match
    class std::_Facet_base * __cdecl std::exchange<class std::_Facet_base *,class std::_Facet_base
   *>(class std::_Facet_base * &,class std::_Facet_base * &&)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

_Facet_base * __cdecl
std::exchange<class_std::_Facet_base*,class_std::_Facet_base*>
          (_Facet_base **param_1,_Facet_base **param_2)

{
  _Facet_base *p_Var1;
  
  p_Var1 = *param_1;
  *param_1 = *param_2;
  return p_Var1;
}

