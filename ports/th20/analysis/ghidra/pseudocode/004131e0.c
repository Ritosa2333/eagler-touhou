/* Entry: 0x004131e0; symbol: FUN_004131e0; body bytes: 151 */

void __thiscall FUN_004131e0(void *this,int param_1,undefined4 param_2)

{
  uint *puVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 local_10 [2];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  puVar1 = (uint *)((int)this + 8);
  uVar2 = FUN_00416270((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (*puVar1 == uVar2) {
    FUN_00540ed5(0x56c8bc);
  }
  uVar3 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  uVar4 = move<>(param_2);
  FUN_00413000(local_10,uVar3,uVar4);
  *puVar1 = *puVar1 + 1;
  FUN_00414eb0(local_10,param_1);
  FUN_00414350(local_10);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

