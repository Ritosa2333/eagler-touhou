/* Entry: 0x004182a0; symbol: FUN_004182a0; body bytes: 221 */

void __thiscall FUN_004182a0(void *this,uint param_1,undefined4 param_2,uint *param_3)

{
  uint uVar1;
  undefined4 uVar2;
  uint *puVar3;
  undefined4 local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_00411280((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar1 < param_1) {
    FUN_0040fb00();
  }
  uVar1 = *(uint *)((int)this + 0x14);
  local_c = FUN_0040ed70(this,param_1);
  uVar2 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  local_10 = FUN_0040c890(uVar2,&local_c);
  FUN_0040e5e0();
  *(uint *)((int)this + 0x10) = param_1;
  *(uint *)((int)this + 0x14) = local_c;
  puVar3 = (uint *)move<>(local_10);
  FID_conflict_operator__(puVar3,param_1,param_3);
  if (uVar1 < 8) {
    FUN_0040cd70(this,&local_10);
  }
  else {
                    /* WARNING: Load size is inaccurate */
    FUN_0040f060(uVar2,*this,uVar1);
    *(undefined4 *)this = local_10;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

