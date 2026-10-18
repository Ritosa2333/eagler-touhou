/* Entry: 0x0053e4b0; symbol: FUN_0053e4b0; body bytes: 166 */

void __thiscall FUN_0053e4b0(void *this,int param_1,int param_2,int param_3)

{
  void *this_00;
  int *piVar1;
  
  this_00 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                  *)this);
  piVar1 = (int *)((int)this + 4);
  FUN_0040e5e0();
  if (*piVar1 != 0) {
    std::_Adl_verify_range<char*,char_const*>((char **)*piVar1,*(char ***)((int)this + 8));
    FUN_00498140(this_00,*piVar1,(*(int *)((int)this + 0xc) - *piVar1) / 0x38);
  }
  *piVar1 = param_1;
  *(int *)((int)this + 8) = param_2 * 0x38 + param_1;
  *(int *)((int)this + 0xc) = param_3 * 0x38 + param_1;
  return;
}

