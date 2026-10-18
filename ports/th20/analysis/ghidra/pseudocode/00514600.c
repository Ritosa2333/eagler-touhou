/* Entry: 0x00514600; symbol: FUN_00514600; body bytes: 301 */

void __thiscall FUN_00514600(void *this,uint param_1)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  int iVar4;
  void *pvVar5;
  uint *puVar6;
  uint *puVar7;
  uint uVar8;
  uint local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  puVar3 = (uint *)((int)this + 4);
  uVar8 = *(uint *)((int)this + 0x14);
  iVar4 = FUN_0044dd60((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (iVar4 - uVar8 < param_1) {
    FUN_0040fb00();
  }
  uVar1 = *(uint *)((int)this + 0x18);
  local_c = FUN_0044b270(this,uVar8 + param_1);
  pvVar5 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  local_10 = FUN_00446ec0(pvVar5,&local_c);
  FUN_0040e5e0();
  *(uint *)((int)this + 0x14) = uVar8 + param_1;
  *(uint *)((int)this + 0x18) = local_c;
  puVar6 = (uint *)move<>(local_10);
  if (uVar1 < 0x10) {
    FID_conflict_operator__(puVar6,puVar3,uVar8);
    FUN_0040cd70(puVar3,&local_10);
  }
  else {
    uVar2 = *puVar3;
    puVar7 = (uint *)move<>(uVar2);
    FID_conflict_operator__(puVar6,puVar7,uVar8);
    FUN_0044b380(pvVar5,uVar2,uVar1);
    *puVar3 = local_10;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

