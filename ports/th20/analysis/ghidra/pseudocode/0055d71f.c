/* Entry: 0x0055d71f; symbol: FUN_0055d71f; body bytes: 214 */

int __cdecl
FUN_0055d71f(uint *param_1,uint *param_2,uint param_3,LPCWSTR param_4,uint param_5,int *param_6)

{
  int iVar1;
  uint uVar2;
  int local_8;
  
  local_8 = 0;
  if (param_2 == (uint *)0x0) {
    if (param_3 != 0) goto LAB_0055d761;
  }
  else {
    if (param_3 == 0) {
LAB_0055d761:
      *(undefined1 *)(param_6 + 7) = 1;
      param_6[6] = 0x16;
      FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_6);
      return 0x16;
    }
    *(undefined1 *)param_2 = 0;
  }
  if (param_1 != (uint *)0x0) {
    *param_1 = 0;
  }
  uVar2 = param_5;
  if (param_3 < param_5) {
    uVar2 = param_3;
  }
  if (uVar2 < 0x80000000) {
    iVar1 = FUN_0055d7f5(param_2,param_4,uVar2,param_6);
    if (iVar1 == -1) {
      if (param_2 != (uint *)0x0) {
        *(undefined1 *)param_2 = 0;
      }
      if ((char)param_6[7] == '\0') {
        return 0;
      }
      return param_6[6];
    }
    uVar2 = iVar1 + 1;
    if (param_2 != (uint *)0x0) {
      if (param_3 < uVar2) {
        if (param_5 != 0xffffffff) {
          *(undefined1 *)param_2 = 0;
          local_8 = 0x22;
          goto LAB_0055d7bd;
        }
        local_8 = 0x50;
        uVar2 = param_3;
      }
      *(undefined1 *)((uVar2 - 1) + (int)param_2) = 0;
    }
    if (param_1 != (uint *)0x0) {
      *param_1 = uVar2;
    }
  }
  else {
    local_8 = 0x16;
LAB_0055d7bd:
    param_6[6] = local_8;
    *(undefined1 *)(param_6 + 7) = 1;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_6);
  }
  return local_8;
}

