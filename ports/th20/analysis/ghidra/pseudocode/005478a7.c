/* Entry: 0x005478a7; symbol: FUN_005478a7; body bytes: 160 */

FARPROC __cdecl FUN_005478a7(int param_1,LPCSTR param_2,int *param_3,int *param_4)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  FARPROC pFVar4;
  HMODULE hLibModule;
  
  piVar1 = (int *)(&DAT_005e52f8 + param_1 * 4);
  pFVar4 = (FARPROC)*piVar1;
  if (pFVar4 == (FARPROC)0xffffffff) {
    pFVar4 = (FARPROC)0x0;
  }
  else if (pFVar4 == (FARPROC)0x0) {
    for (; param_3 != param_4; param_3 = param_3 + 1) {
      iVar3 = *param_3;
      hLibModule = *(HMODULE *)(&DAT_005e52ec + iVar3 * 4);
      if (hLibModule == (HMODULE)0x0) {
        hLibModule = try_load_library_from_system_directory
                               ((wchar_t *)(&PTR_u_api_ms_win_core_fibers_l1_1_1_00597b44)[iVar3]);
        piVar2 = (int *)(&DAT_005e52ec + iVar3 * 4);
        if (hLibModule != (HINSTANCE__ *)0x0) {
          LOCK();
          iVar3 = *piVar2;
          *piVar2 = (int)hLibModule;
          UNLOCK();
          if (iVar3 != 0) {
            FreeLibrary(hLibModule);
          }
          goto LAB_00547930;
        }
        LOCK();
        *piVar2 = -1;
        UNLOCK();
      }
      else if (hLibModule != (HMODULE)0xffffffff) {
LAB_00547930:
        pFVar4 = GetProcAddress(hLibModule,param_2);
        if (pFVar4 != (FARPROC)0x0) {
          LOCK();
          *piVar1 = (int)pFVar4;
          UNLOCK();
          return pFVar4;
        }
        break;
      }
    }
    LOCK();
    *piVar1 = -1;
    UNLOCK();
    pFVar4 = (FARPROC)0x0;
  }
  return pFVar4;
}

