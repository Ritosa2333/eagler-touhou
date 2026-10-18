/* Entry: 0x004d2360; symbol: FUN_004d2360; body bytes: 176 */

undefined4 __fastcall FUN_004d2360(int param_1)

{
  char **ppcVar1;
  undefined4 local_c;
  
  local_c = *(char ***)(param_1 + 0x1330);
  while (local_c != (char **)0x0) {
    ppcVar1 = (char **)*local_c;
    FUN_00413b70(DAT_005b8894,local_c);
    local_c = ppcVar1;
  }
  *(undefined4 *)(param_1 + 0x1330) = 0;
  if (*(int *)(param_1 + 0x132c) != 0) {
    FUN_0041f670(*(LPVOID *)(param_1 + 0x132c));
    *(undefined4 *)(param_1 + 0x132c) = 0;
  }
  if (*(int *)(param_1 + 0x1328) != 0) {
    FUN_0041f670(*(LPVOID *)(param_1 + 0x1328));
    *(undefined4 *)(param_1 + 0x1328) = 0;
  }
  return 0;
}

