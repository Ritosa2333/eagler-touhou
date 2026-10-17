/* Entry: 0x00412750; symbol: FUN_00412750; body bytes: 95 */

void __thiscall FUN_00412750(void *this,int param_1)

{
  char *pcVar1;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var2;
  
  if (*(char *)((int)this + 0x436) != '\0') {
    pcVar1 = (char *)FUN_00411c10((void *)((int)this + 0x420),param_1);
    *pcVar1 = *pcVar1 + -1;
    p_Var2 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040bbc0(this,param_1);
    FUN_0040c660(p_Var2);
  }
  return;
}

