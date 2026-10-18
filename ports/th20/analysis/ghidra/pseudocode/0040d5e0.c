/* Entry: 0x0040d5e0; symbol: FUN_0040d5e0; body bytes: 288 */

void __thiscall FUN_0040d5e0(void *this,uint param_1)

{
  uint uVar1;
  LPVOID pvVar2;
  int iVar3;
  allocator<char> *paVar4;
  uint *puVar5;
  uint *puVar6;
  int iVar7;
  undefined4 local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar7 = *(int *)((int)this + 0x10);
  iVar3 = FUN_00411200((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if ((uint)(iVar3 - iVar7) < param_1) {
    FUN_0040fb00();
  }
  uVar1 = *(uint *)((int)this + 0x14);
  local_c = FUN_0040ecc0(this,iVar7 + param_1);
  paVar4 = (allocator<char> *)
           FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  local_10 = FUN_0040c800(paVar4,&local_c);
  FUN_0040e5e0();
  *(uint *)((int)this + 0x10) = iVar7 + param_1;
  *(uint *)((int)this + 0x14) = local_c;
  puVar5 = (uint *)move<>(local_10);
  if (uVar1 < 0x10) {
    FUN_0040e9c0(puVar5,(uint *)this,iVar7);
    FUN_0040cd70(this,&local_10);
  }
  else {
                    /* WARNING: Load size is inaccurate */
    pvVar2 = *this;
    puVar6 = (uint *)move<>(pvVar2);
    FUN_0040e9c0(puVar5,puVar6,iVar7);
    FUN_0040f040(paVar4,pvVar2,uVar1);
    *(undefined4 *)this = local_10;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

