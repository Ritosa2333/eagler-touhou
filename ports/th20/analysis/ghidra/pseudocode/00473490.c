/* Entry: 0x00473490; symbol: FUN_00473490; body bytes: 250 */

void __fastcall FUN_00473490(int param_1)

{
  int local_14;
  int local_10;
  short *local_c;
  
  FUN_00412d80(*(int *)(param_1 + 8));
  FUN_00412d80(*(int *)(param_1 + 0xc));
  FUN_00412d80(*(int *)(param_1 + 0x3368));
  local_14 = 0;
  for (local_10 = 0; local_10 < **(short **)(param_1 + 0x3324); local_10 = local_10 + 1) {
    *(undefined1 *)(*(int *)(*(int *)(param_1 + 0x3328) + local_10 * 4) + 3) = 1;
    for (local_c = (short *)(*(int *)(*(int *)(param_1 + 0x3328) + local_10 * 4) + 0x30);
        -1 < *local_c; local_c = (short *)FUN_00471160((int)local_c,(int)local_c[1])) {
      FUN_00438380(*(void **)(param_1 + 0x3334),
                   (void *)(local_14 * 0x5e4 + *(int *)(param_1 + 0x3320)),(int)local_c[2]);
      local_c[3] = (short)local_14;
      local_14 = local_14 + 1;
    }
  }
  *(undefined4 *)(param_1 + 0x20) = 0;
  return;
}

