/* Entry: 0x0055e146; symbol: FUN_0055e146; body bytes: 198 */

int __cdecl
FUN_0055e146(LPCWSTR param_1,
            __crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing> *param_2
            ,undefined4 param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  DWORD DVar3;
  __acrt_ptd *p_Var4;
  
  if (param_1 == (LPCWSTR)0x0) {
    __crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing>::_deallocate
              (param_2);
    *(undefined4 *)(param_2 + 8) = 0;
    *(undefined4 *)(param_2 + 0xc) = 0;
  }
  else {
    if (*param_1 != L'\0') {
      uVar2 = FUN_0055de4b(param_4,0,param_1,-1,(LPSTR)0x0,0,(LPCSTR)0x0,(LPBOOL)0x0);
      if (uVar2 == 0) {
        DVar3 = GetLastError();
        ___acrt_errno_map_os_error(DVar3);
        p_Var4 = FUN_005516c1();
        return *(int *)p_Var4;
      }
      if ((*(uint *)(param_2 + 0xc) < uVar2) && (iVar1 = allocate(param_2,uVar2), iVar1 != 0)) {
        return iVar1;
      }
      iVar1 = FUN_0055273f(param_4,param_1,*(LPSTR *)(param_2 + 8),*(int *)(param_2 + 0xc));
      if (iVar1 == 0) {
        DVar3 = GetLastError();
        ___acrt_errno_map_os_error(DVar3);
        p_Var4 = FUN_005516c1();
        return *(int *)p_Var4;
      }
      *(int *)(param_2 + 0x10) = iVar1 + -1;
      return 0;
    }
    if ((*(int *)(param_2 + 0xc) == 0) && (iVar1 = allocate(param_2,1), iVar1 != 0)) {
      return iVar1;
    }
    **(undefined1 **)(param_2 + 8) = 0;
  }
  *(undefined4 *)(param_2 + 0x10) = 0;
  return 0;
}

