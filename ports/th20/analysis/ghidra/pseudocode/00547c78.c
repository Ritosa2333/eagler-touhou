/* Entry: 0x00547c78; symbol: common_end_thread; body bytes: 86 */

/* Library Function - Single Match
    void __cdecl common_end_thread(unsigned int)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl common_end_thread(uint param_1)

{
  int iVar1;
  HANDLE hObject;
  HMODULE hLibModule;
  __acrt_ptd *p_Var2;
  void *extraout_ECX;
  
  p_Var2 = FUN_00557a13();
  if ((p_Var2 != (__acrt_ptd *)0x0) && (iVar1 = *(int *)(p_Var2 + 0x360), iVar1 != 0)) {
    if (*(char *)(iVar1 + 0x10) != '\0') {
      FUN_00558304(extraout_ECX);
    }
    hObject = *(HANDLE *)(iVar1 + 8);
    if ((hObject != (HANDLE)0xffffffff) && (hObject != (HANDLE)0x0)) {
      CloseHandle(hObject);
    }
    hLibModule = *(HMODULE *)(iVar1 + 0xc);
    if ((hLibModule != (HMODULE)0xffffffff) && (hLibModule != (HMODULE)0x0)) {
                    /* WARNING: Subroutine does not return */
      FreeLibraryAndExitThread(hLibModule,param_1);
    }
  }
                    /* WARNING: Subroutine does not return */
  ExitThread(param_1);
}

