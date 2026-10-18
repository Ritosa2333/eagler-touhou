/* Entry: 0x00547e5d; symbol: FUN_00547e5d; body bytes: 70 */

__acrt_ptd * __fastcall FUN_00547e5d(undefined4 *param_1)

{
  int iVar1;
  __acrt_ptd *p_Var2;
  undefined4 *local_8;
  
  local_8 = param_1;
  local_8 = (undefined4 *)GetLastError();
  if (*(char *)(param_1 + 2) == '\0') {
    iVar1 = 0;
    *(undefined1 *)(param_1 + 2) = 1;
    param_1[1] = 0;
  }
  else {
    iVar1 = param_1[1];
  }
  p_Var2 = FUN_00557a9f(&local_8,iVar1);
  *param_1 = p_Var2;
  SetLastError((DWORD)local_8);
  return p_Var2;
}

