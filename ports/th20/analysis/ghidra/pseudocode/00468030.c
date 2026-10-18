/* Entry: 0x00468030; symbol: FUN_00468030; body bytes: 333 */

void __thiscall FUN_00468030(void *this,uint *param_1,uint param_2)

{
  uint uVar1;
  void *pvVar2;
  uint *puVar3;
  int iVar4;
  undefined1 *puVar5;
  undefined1 local_1a;
  undefined1 local_19;
  uint *local_18;
  undefined4 local_14;
  uint local_10;
  undefined1 local_a;
  undefined1 local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = (uint *)((int)this + 4);
  uVar1 = FUN_0044dd60((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar1 < param_2) {
    FUN_0040fb00();
  }
  pvVar2 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  local_a = 0;
  FUN_0040de90(&local_9);
  if (param_2 < 0x10) {
    local_18[4] = param_2;
    local_18[5] = 0xf;
    FUN_0040ffd0(local_18,param_1,param_2);
    local_19 = 0;
    FUN_0040fe90((undefined1 *)((int)local_18 + param_2),&local_19);
    FUN_0040e5e0();
  }
  else {
    uVar1 = FUN_0044dd60((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)this);
    local_10 = FUN_0040ed00(param_2,0xf,uVar1);
    local_14 = FUN_00446ec0(pvVar2,&local_10);
    FUN_0040cd70(local_18,&local_14);
    local_18[4] = param_2;
    local_18[5] = local_10;
    uVar1 = param_2;
    puVar3 = (uint *)move<>(local_14);
    FUN_0040ffd0(puVar3,param_1,uVar1);
    local_1a = 0;
    puVar5 = &local_1a;
    iVar4 = move<>(local_14);
    FUN_0040fe90((undefined1 *)(iVar4 + param_2),puVar5);
    FUN_0040e5e0();
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

