/* Entry: 0x004228b0; symbol: FUN_004228b0; body bytes: 583 */

void __fastcall FUN_004228b0(uint *param_1)

{
  int *piVar1;
  uint *puVar2;
  undefined4 *puVar3;
  uint local_14;
  uint local_10;
  uint local_c;
  
  local_10 = 1;
  local_14 = *param_1;
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[0xac] = 0;
  for (local_c = 0; local_c < 0x20; local_c = local_c + 1) {
    if ((local_14 & 1) == 0) {
      puVar3 = (undefined4 *)FUN_00414580(param_1 + 6,local_c);
      *puVar3 = 0;
      puVar3 = (undefined4 *)FUN_00414580(param_1 + 0x26,local_c);
      *puVar3 = 0;
      puVar3 = (undefined4 *)FUN_00414580(param_1 + 0x66,local_c);
      *puVar3 = 0;
    }
    else {
      piVar1 = (int *)FUN_00414580(param_1 + 6,local_c);
      *piVar1 = *piVar1 + 1;
      piVar1 = (int *)FUN_00414580(param_1 + 0x26,local_c);
      *piVar1 = *piVar1 + 1;
      piVar1 = (int *)FUN_00414580(param_1 + 0x66,local_c);
      *piVar1 = *piVar1 + 1;
      puVar2 = (uint *)FUN_00414580(param_1 + 6,local_c);
      if (7 < *puVar2) {
        param_1[0xac] = param_1[0xac] | local_10;
      }
      puVar2 = (uint *)FUN_00414580(param_1 + 6,local_c);
      if (0x19 < *puVar2) {
        param_1[2] = param_1[2] | local_10;
        piVar1 = (int *)FUN_00414580(param_1 + 6,local_c);
        *piVar1 = *piVar1 + -8;
      }
      puVar2 = (uint *)FUN_00414580(param_1 + 0x26,local_c);
      if (0x19 < *puVar2) {
        param_1[3] = param_1[3] | local_10;
        piVar1 = (int *)FUN_00414580(param_1 + 0x26,local_c);
        *piVar1 = *piVar1 + -0xc;
      }
    }
    local_14 = local_14 >> 1;
    local_10 = local_10 << 1;
  }
  param_1[4] = (*param_1 ^ param_1[1]) & *param_1;
  param_1[5] = (*param_1 ^ param_1[1]) & ~*param_1;
  return;
}

