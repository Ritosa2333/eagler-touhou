/* Entry: 0x00447050; symbol: FUN_00447050; body bytes: 229 */

void __thiscall FUN_00447050(void *this,uint param_1,undefined4 param_2,uint *param_3)

{
  uint uVar1;
  void *pvVar2;
  uint *puVar3;
  undefined4 local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_0044dd60((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar1 < param_1) {
    FUN_0040fb00();
  }
  uVar1 = *(uint *)((int)this + 0x18);
  local_c = FUN_0044b270(this,param_1);
  pvVar2 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  local_10 = FUN_00446ec0(pvVar2,&local_c);
  FUN_0040e5e0();
  *(uint *)((int)this + 0x14) = param_1;
  *(uint *)((int)this + 0x18) = local_c;
  puVar3 = (uint *)move<>(local_10);
  FUN_004495c0(puVar3,param_1,param_3);
  if (uVar1 < 0x10) {
    FUN_0040cd70((int)this + 4,&local_10);
  }
  else {
    FUN_0044b380(pvVar2,*(undefined4 *)((int)this + 4),uVar1);
    *(undefined4 *)((int)this + 4) = local_10;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

