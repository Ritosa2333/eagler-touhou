/* Entry: 0x00417ec0; symbol: FUN_00417ec0; body bytes: 340 */

void __thiscall FUN_00417ec0(void *this,uint *param_1,uint param_2)

{
  uint uVar1;
  undefined4 uVar2;
  uint *puVar3;
  int iVar4;
  undefined2 *puVar5;
  undefined2 local_20;
  undefined2 local_1e;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_1c;
  uint *local_18;
  undefined4 local_14;
  uint local_10;
  undefined1 local_a;
  undefined1 local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1c = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)this;
  local_18 = (uint *)this;
  uVar1 = FUN_00411280((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar1 < param_2) {
    FUN_0040fb00();
  }
  uVar2 = FUN_0040f160(local_1c);
  local_a = 0;
  FUN_0040de90(&local_9);
  if (param_2 < 8) {
    local_18[4] = param_2;
    local_18[5] = 7;
    FUN_00410030(local_18,param_1,param_2);
    local_1e = 0;
    FUN_0040ff00((undefined2 *)((int)local_18 + param_2 * 2),&local_1e);
    FUN_0040e5e0();
  }
  else {
    uVar1 = FUN_00411280(local_1c);
    local_10 = FUN_0040edb0(param_2,7,uVar1);
    local_14 = FUN_0040c890(uVar2,&local_10);
    FUN_0040cd70(local_18,&local_14);
    local_18[4] = param_2;
    local_18[5] = local_10;
    uVar1 = param_2;
    puVar3 = (uint *)move<>(local_14);
    FUN_00410030(puVar3,param_1,uVar1);
    local_20 = 0;
    puVar5 = &local_20;
    iVar4 = move<>(local_14);
    FUN_0040ff00((undefined2 *)(iVar4 + param_2 * 2),puVar5);
    FUN_0040e5e0();
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

