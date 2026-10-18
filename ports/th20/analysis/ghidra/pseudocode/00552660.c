/* Entry: 0x00552660; symbol: FUN_00552660; body bytes: 194 */

int __cdecl FUN_00552660(LPCWSTR param_1,int param_2,undefined4 param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  DWORD DVar3;
  __acrt_ptd *p_Var4;
  
  if (param_1 == (LPCWSTR)0x0) {
    FUN_00552804(param_2);
    iVar1 = 0;
  }
  else if (*param_1 == L'\0') {
    if ((*(int *)(param_2 + 0xc) != 0) || (iVar1 = allocate(param_2), iVar1 == 0)) {
      **(undefined1 **)(param_2 + 8) = 0;
      iVar1 = 0;
      *(undefined4 *)(param_2 + 0x10) = 0;
    }
  }
  else {
    uVar2 = FUN_0055de4b(param_4,0,param_1,-1,(LPSTR)0x0,0,(LPCSTR)0x0,(LPBOOL)0x0);
    if (uVar2 == 0) {
      DVar3 = GetLastError();
      ___acrt_errno_map_os_error(DVar3);
      p_Var4 = FUN_005516c1();
      iVar1 = *(int *)p_Var4;
    }
    else if ((uVar2 <= *(uint *)(param_2 + 0xc)) || (iVar1 = allocate(param_2), iVar1 == 0)) {
      iVar1 = FUN_0055273f(param_4,param_1,*(LPSTR *)(param_2 + 8),*(int *)(param_2 + 0xc));
      if (iVar1 == 0) {
        DVar3 = GetLastError();
        ___acrt_errno_map_os_error(DVar3);
        p_Var4 = FUN_005516c1();
        iVar1 = *(int *)p_Var4;
      }
      else {
        *(int *)(param_2 + 0x10) = iVar1 + -1;
        iVar1 = 0;
      }
    }
  }
  return iVar1;
}

