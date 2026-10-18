/* Entry: 0x004145d0; symbol: FUN_004145d0; body bytes: 360 */

void __fastcall FUN_004145d0(int *param_1)

{
  bool bVar1;
  int iVar2;
  uint *local_24;
  int local_20;
  int local_1c;
  int local_14;
  uint *local_10;
  int *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  (**(code **)(*(int *)param_1[1] + 0x48))(param_1[1],0,&local_c);
  (**(code **)(*local_c + 0x34))(local_c,&local_14,param_1 + 2,0);
  local_24 = local_10;
  local_1c = FUN_004156a0(*param_1);
  iVar2 = FUN_004156c0(*param_1);
  local_1c = iVar2 * param_1[7] + local_1c;
  for (local_20 = 0; local_20 < param_1[5] - param_1[3]; local_20 = local_20 + 1) {
    FUN_00543e20(local_24,(uint *)(local_1c + param_1[6]),(param_1[8] - param_1[6]) * 4);
    local_24 = (uint *)((int)local_24 + local_14);
    iVar2 = FUN_004156c0(*param_1);
    local_1c = iVar2 + local_1c;
  }
  (**(code **)(*local_c + 0x38))(local_c);
  if (local_c != (int *)0x0) {
    (**(code **)(*local_c + 8))(local_c);
    local_c = (int *)0x0;
  }
  FUN_00416360(*param_1);
  FUN_00413be0((void *)*param_1);
  if (param_1[10] != 0) {
    *(undefined1 *)param_1[10] = 1;
  }
  bVar1 = FUN_004145a0((int)(param_1 + 0xc));
  if (bVar1) {
    FUN_00414740((int)(param_1 + 0xc));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

