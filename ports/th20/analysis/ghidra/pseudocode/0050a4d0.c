/* Entry: 0x0050a4d0; symbol: FUN_0050a4d0; body bytes: 475 */

void __fastcall FUN_0050a4d0(int param_1)

{
  int *piVar1;
  uint *puVar2;
  undefined4 *puVar3;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_10 = 1;
  local_14 = *(uint *)(param_1 + 0x29c);
  *(undefined4 *)(param_1 + 0x2a4) = 0;
  *(undefined4 *)(param_1 + 0x2b4) = 0;
  for (local_c = 0; local_c < 0x20; local_c = local_c + 1) {
    if ((local_14 & 1) == 0) {
      puVar3 = (undefined4 *)FUN_00414580((void *)(param_1 + 0x118),local_c);
      *puVar3 = 0;
      puVar3 = (undefined4 *)FUN_00414580((void *)(param_1 + 0x218),local_c);
      *puVar3 = 0;
    }
    else {
      piVar1 = (int *)FUN_00414580((void *)(param_1 + 0x118),local_c);
      *piVar1 = *piVar1 + 1;
      piVar1 = (int *)FUN_00414580((void *)(param_1 + 0x218),local_c);
      *piVar1 = *piVar1 + 1;
      puVar2 = (uint *)FUN_00414580((void *)(param_1 + 0x118),local_c);
      if (7 < *puVar2) {
        *(uint *)(param_1 + 0x2b4) = *(uint *)(param_1 + 0x2b4) | local_10;
      }
      puVar2 = (uint *)FUN_00414580((void *)(param_1 + 0x118),local_c);
      if (0x19 < *puVar2) {
        *(uint *)(param_1 + 0x2a4) = *(uint *)(param_1 + 0x2a4) | local_10;
        piVar1 = (int *)FUN_00414580((void *)(param_1 + 0x118),local_c);
        *piVar1 = *piVar1 + -8;
      }
    }
    local_14 = local_14 >> 1;
    local_10 = local_10 << 1;
  }
  *(uint *)(param_1 + 0x2a8) =
       (*(uint *)(param_1 + 0x29c) ^ *(uint *)(param_1 + 0x2a0)) & *(uint *)(param_1 + 0x29c);
  *(uint *)(param_1 + 0x2ac) =
       (*(uint *)(param_1 + 0x29c) ^ *(uint *)(param_1 + 0x2a0)) & ~*(uint *)(param_1 + 0x29c);
  return;
}

