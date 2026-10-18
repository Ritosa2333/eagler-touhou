/* Entry: 0x00497630; symbol: FUN_00497630; body bytes: 84 */

uint __thiscall FUN_00497630(void *this,uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = FUN_00497ee0((int)this);
  uVar2 = FUN_0049c1a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if ((uVar1 <= uVar2 - (uVar1 >> 1)) && (uVar2 = (uVar1 >> 1) + uVar1, uVar2 < param_1)) {
    uVar2 = param_1;
  }
  return uVar2;
}

