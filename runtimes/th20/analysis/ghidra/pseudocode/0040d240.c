/* Entry: 0x0040d240; symbol: FUN_0040d240; body bytes: 304 */

void __thiscall FUN_0040d240(void *this,uint param_1,undefined4 param_2,uint *param_3,uint param_4)

{
  uint uVar1;
  LPVOID pvVar2;
  int iVar3;
  allocator<char> *paVar4;
  uint *puVar5;
  uint *puVar6;
  uint uVar7;
  undefined4 local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar7 = *(uint *)((int)this + 0x10);
  iVar3 = FUN_00411200((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (iVar3 - uVar7 < param_1) {
    FUN_0040fb00();
  }
  uVar1 = *(uint *)((int)this + 0x14);
  local_c = FUN_0040ecc0(this,uVar7 + param_1);
  paVar4 = (allocator<char> *)
           FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  local_10 = FUN_0040c800(paVar4,&local_c);
  FUN_0040e5e0();
  *(uint *)((int)this + 0x10) = uVar7 + param_1;
  *(uint *)((int)this + 0x14) = local_c;
  puVar5 = (uint *)move<>(local_10);
  if (uVar1 < 0x10) {
    FID_conflict_operator__(puVar5,(uint *)this,uVar7,param_3,param_4);
    FUN_0040cd70(this,&local_10);
  }
  else {
                    /* WARNING: Load size is inaccurate */
    pvVar2 = *this;
    puVar6 = (uint *)move<>(pvVar2);
    FID_conflict_operator__(puVar5,puVar6,uVar7,param_3,param_4);
    FUN_0040f040(paVar4,pvVar2,uVar1);
    *(undefined4 *)this = local_10;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

