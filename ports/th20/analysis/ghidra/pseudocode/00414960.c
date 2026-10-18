/* Entry: 0x00414960; symbol: FUN_00414960; body bytes: 165 */

void __thiscall FUN_00414960(void *this,int param_1,int param_2,int param_3)

{
  void *this_00;
  int *piVar1;
  
  this_00 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                  *)this);
  piVar1 = (int *)((int)this + 4);
  FUN_0040e5e0();
  if (*piVar1 != 0) {
    std::_Adl_verify_range<char*,char_const*>((char **)*piVar1,*(char ***)((int)this + 8));
    FUN_004154c0(this_00,*piVar1,*(int *)((int)this + 0xc) - *piVar1 >> 3);
  }
  *piVar1 = param_1;
  *(int *)((int)this + 8) = param_1 + param_2 * 8;
  *(int *)((int)this + 0xc) = param_1 + param_3 * 8;
  return;
}

