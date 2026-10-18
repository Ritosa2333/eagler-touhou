/* Entry: 0x00412550; symbol: FUN_00412550; body bytes: 94 */

void __thiscall FUN_00412550(void *this,int param_1)

{
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var1;
  char *pcVar2;
  
  if (*(char *)((int)this + 0x436) != '\0') {
    p_Var1 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040bbc0(this,param_1);
    FUN_0040c470(p_Var1);
    pcVar2 = (char *)FUN_00411c10((void *)((int)this + 0x420),param_1);
    *pcVar2 = *pcVar2 + '\x01';
  }
  return;
}

