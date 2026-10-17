/* Entry: 0x004b8bf0; symbol: FUN_004b8bf0; body bytes: 412 */

void __thiscall FUN_004b8bf0(void *this,int param_1,int param_2,int param_3)

{
  int *piVar1;
  undefined4 *puVar2;
  uint local_18;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  piVar1 = (int *)FUN_00414580((void *)((int)this + 0x50),0);
  if (*piVar1 != 0) {
    FUN_00422dd0(local_14,0,0,0);
    local_14[0] = (float)(7 - param_3) * DAT_0056fd48;
    for (local_18 = 0; local_18 < 7; local_18 = local_18 + 1) {
      puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x50),local_18);
      FUN_0045dd60((void *)*puVar2,local_14);
    }
    for (local_18 = 0; (int)local_18 < param_1; local_18 = local_18 + 1) {
      puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x50),local_18);
      FUN_00486330((void *)*puVar2);
    }
    if ((int)local_18 < param_3) {
      puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x50),local_18);
      FUN_004b85a0((void *)*puVar2,*(int *)(&DAT_00570780 + param_2 * 4));
      local_18 = local_18 + 1;
    }
    for (; (int)local_18 < param_3; local_18 = local_18 + 1) {
      puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x50),local_18);
      FUN_00486370((void *)*puVar2);
    }
    for (; local_18 < 7; local_18 = local_18 + 1) {
      puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x50),local_18);
      FUN_004b8580((void *)*puVar2);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

