/* Entry: 0x0040b850; symbol: FUN_0040b850; body bytes: 46 */

undefined4 * FUN_0040b850(_Facet_base **param_1)

{
  _Facet_base *p_Var1;
  _Facet_base *local_c;
  undefined4 *local_8;
  
  local_c = (_Facet_base *)0x0;
  p_Var1 = std::exchange<class_std::_Facet_base*,class_std::_Facet_base*>(param_1,&local_c);
  *local_8 = p_Var1;
  return local_8;
}

