/* Entry: 0x0053a210; symbol: FUN_0053a210; body bytes: 314 */

/* WARNING: Removing unreachable block (ram,0x0053a314) */
/* WARNING: Removing unreachable block (ram,0x0053a32f) */
/* WARNING: Removing unreachable block (ram,0x0053a320) */
/* WARNING: Removing unreachable block (ram,0x0053a336) */

void FUN_0053a210(char *param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  char *pcVar2;
  int local_14;
  char *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = FUN_00539b90("D:\\cygwin\\home\\zun\\prog\\th20\\src\\pack\\_ArcMngr.cpp:385 ARCFILE_INFO"
                       ,param_2 + 1);
  if (iVar1 != 0) {
    local_c = param_1;
    for (local_14 = 0; local_14 < param_2; local_14 = local_14 + 1) {
      pcVar2 = FUN_00539e60(local_c);
      *(char **)(iVar1 + local_14 * 0x10) = pcVar2;
      FUN_0053a680((int *)&local_c);
      *(undefined4 *)(iVar1 + 4 + local_14 * 0x10) = *(undefined4 *)local_c;
      FUN_0053a650((int *)&local_c);
      *(undefined4 *)(iVar1 + 8 + local_14 * 0x10) = *(undefined4 *)local_c;
      FUN_0053a650((int *)&local_c);
      *(undefined4 *)(iVar1 + 0xc + local_14 * 0x10) = *(undefined4 *)local_c;
      FUN_0053a650((int *)&local_c);
    }
    *(undefined4 *)(iVar1 + 4 + param_2 * 0x10) = param_3;
    *(undefined4 *)(iVar1 + 8 + param_2 * 0x10) = 0;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

