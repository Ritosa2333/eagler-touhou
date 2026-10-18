/* Entry: 0x0049c220; symbol: FUN_0049c220; body bytes: 58 */

void __thiscall FUN_0049c220(void *this,uint param_1)

{
  uint uVar1;
  
  uVar1 = FUN_00497eb0((int)this);
  if (uVar1 < param_1) {
    uVar1 = FUN_0049c160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)this);
    if (uVar1 < param_1) {
      FUN_00414fe0();
    }
    FUN_00489cb0(this,&param_1);
  }
  return;
}

