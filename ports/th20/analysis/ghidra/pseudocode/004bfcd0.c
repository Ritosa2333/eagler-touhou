/* Entry: 0x004bfcd0; symbol: FUN_004bfcd0; body bytes: 131 */

void __cdecl
FUN_004bfcd0(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5
            )

{
  uint uVar1;
  int iVar2;
  
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar2 = FUN_004c0f70(param_1);
  if (*(int *)(iVar2 + 0x4c) == 0) {
    FUN_00504af0(iVar2);
  }
  else {
    (**(code **)(iVar2 + 0x4c))(param_2,param_3,param_4,param_5);
  }
  FUN_005429ee(uVar1 ^ (uint)&stack0xfffffffc);
  return;
}

