/* Entry: 0x004a9960; symbol: FUN_004a9960; body bytes: 166 */

void __thiscall FUN_004a9960(void *this,int param_1,int param_2,int param_3)

{
  void *this_00;
  int *piVar1;
  
  this_00 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                  *)this);
  piVar1 = (int *)((int)this + 4);
  FUN_0040e5e0();
  if (*piVar1 != 0) {
    FUN_004a18a0(*piVar1,*(int *)((int)this + 8),this_00);
    FUN_004aa8d0(this_00,*piVar1,(*(int *)((int)this + 0xc) - *piVar1) / 0x1c);
  }
  *piVar1 = param_1;
  *(int *)((int)this + 8) = param_2 * 0x1c + param_1;
  *(int *)((int)this + 0xc) = param_3 * 0x1c + param_1;
  return;
}

