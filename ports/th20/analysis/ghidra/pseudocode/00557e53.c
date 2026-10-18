/* Entry: 0x00557e53; symbol: FUN_00557e53; body bytes: 204 */

FARPROC __thiscall FUN_00557e53(void *this,int param_1,LPCSTR param_2,int *param_3,int *param_4)

{
  int *piVar1;
  int iVar2;
  HMODULE hLibModule;
  BOOL BVar3;
  FARPROC pFVar4;
  FARPROC pFVar5;
  void *local_8;
  
  local_8 = this;
  do {
    if (param_3 == param_4) {
      pFVar5 = (FARPROC)0x0;
LAB_00557ea2:
      ___acrt_lock(0xe);
      BVar3 = VirtualProtect(&DAT_005e6000,0x80,4,(PDWORD)&local_8);
      if (BVar3 != 0) {
        pFVar4 = pFVar5;
        if (pFVar5 == (FARPROC)0x0) {
          pFVar4 = (FARPROC)0xffffffff;
        }
        LOCK();
        (&DAT_005e6000)[param_1] = pFVar4;
        UNLOCK();
        BVar3 = VirtualProtect(&DAT_005e6000,0x80,2,(PDWORD)&local_8);
        if (BVar3 != 0) {
          ___acrt_unlock(0xe);
          return pFVar5;
        }
      }
                    /* WARNING: Subroutine does not return */
      _abort();
    }
    piVar1 = &DAT_005e5600 + *param_3;
    hLibModule = (HMODULE)*piVar1;
    if (hLibModule == (HMODULE)0x0) {
      hLibModule = try_load_library_from_system_directory
                             ((wchar_t *)(&PTR_u_api_ms_win_core_datetime_l1_1_1_005997b0)[*param_3]
                             );
      if (hLibModule != (HINSTANCE__ *)0x0) {
        LOCK();
        iVar2 = *piVar1;
        *piVar1 = (int)hLibModule;
        UNLOCK();
        if (iVar2 != 0) {
          FreeLibrary(hLibModule);
        }
        goto LAB_00557f0c;
      }
      LOCK();
      *piVar1 = -1;
      UNLOCK();
    }
    else if (hLibModule != (HMODULE)0xffffffff) {
LAB_00557f0c:
      pFVar5 = GetProcAddress(hLibModule,param_2);
      goto LAB_00557ea2;
    }
    param_3 = param_3 + 1;
  } while( true );
}

