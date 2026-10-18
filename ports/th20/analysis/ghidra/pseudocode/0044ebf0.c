/* Entry: 0x0044ebf0; symbol: FUN_0044ebf0; body bytes: 336 */

void FUN_0044ebf0(int param_1,int param_2,int *param_3)

{
  int iVar1;
  SIZE_T local_110;
  wchar_t local_10c [130];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (param_3 == (int *)0x0) {
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_0056e428);
  }
  else if (*param_3 == 8) {
    if (((char)param_3[8] == '\0') && (*(char *)((int)param_3 + param_3[4]) != '@')) {
      FUN_00429120(local_10c,L"猥");
      iVar1 = FUN_00410aa0((undefined1 (*) [16])local_10c,&local_110,1);
      if (iVar1 == 0) {
        FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_0056e488);
      }
      else {
        *(SIZE_T *)(*(int *)(param_1 + 0x58) + 8 + param_2 * 0x18) = local_110;
        *(int *)(*(int *)(param_1 + 0x58) + 4 + param_2 * 0x18) = iVar1;
      }
    }
  }
  else {
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_0056e464);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

