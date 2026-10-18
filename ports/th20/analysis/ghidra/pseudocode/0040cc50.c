/* Entry: 0x0040cc50; symbol: FUN_0040cc50; body bytes: 273 */

void __thiscall FUN_0040cc50(void *this,uint *param_1,uint param_2)

{
  uint uVar1;
  undefined4 uVar2;
  uint *puVar3;
  int iVar4;
  undefined4 local_14;
  uint local_10;
  undefined1 local_a;
  undefined1 local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_00411280((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar1 < param_2) {
    FUN_0040fb00();
  }
  uVar2 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  local_a = 0;
  FUN_0040de90(&local_9);
  if (param_2 < 8) {
    *(uint *)((int)this + 0x10) = param_2;
    *(undefined4 *)((int)this + 0x14) = 7;
    FUN_00410030((uint *)this,param_1,8);
    FUN_0040e5e0();
  }
  else {
    uVar1 = FUN_00411280((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)this);
    local_10 = FUN_0040edb0(param_2,7,uVar1);
    local_14 = FUN_0040c890(uVar2,&local_10);
    FUN_0040cd70(this,&local_14);
    *(uint *)((int)this + 0x10) = param_2;
    *(uint *)((int)this + 0x14) = local_10;
    iVar4 = param_2 + 1;
    puVar3 = (uint *)move<>(local_14);
    FUN_00410030(puVar3,param_1,iVar4);
    FUN_0040e5e0();
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

