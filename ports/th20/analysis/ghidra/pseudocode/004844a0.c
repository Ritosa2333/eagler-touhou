/* Entry: 0x004844a0; symbol: FUN_004844a0; body bytes: 200 */

void __thiscall FUN_004844a0(void *this,uint param_1)

{
  void *this_00;
  int *piVar1;
  uint uVar2;
  
  this_00 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                  *)this);
  piVar1 = (int *)((int)this + 4);
  uVar2 = FUN_00485b20((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar2 < param_1) {
    FUN_00414fe0();
  }
  uVar2 = FUN_00484390(this,param_1);
  if (*piVar1 != 0) {
    std::_Adl_verify_range<char*,char_const*>((char **)*piVar1,*(char ***)((int)this + 8));
    FUN_004852b0(this_00,*piVar1,(*(int *)((int)this + 0xc) - *piVar1) / 0x2c);
    *piVar1 = 0;
    *(undefined4 *)((int)this + 8) = 0;
    *(int *)((int)this + 0xc) = 0;
  }
  FUN_00484320(this,uVar2);
  return;
}

