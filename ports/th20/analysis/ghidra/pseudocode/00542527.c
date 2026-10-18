/* Entry: 0x00542527; symbol: _Atexit; body bytes: 47 */

/* Library Function - Single Match
    void __cdecl _Atexit(void (__cdecl*)(void))
   
   Library: Visual Studio 2019 Release */

void __cdecl _Atexit(_func_void *param_1)

{
  code *pcVar1;
  PVOID pvVar2;
  int iVar3;
  BOOL BVar4;
  
  if (DAT_005b2584 != 0) {
    pvVar2 = EncodePointer(param_1);
    DAT_005b2584 = DAT_005b2584 + -1;
    *(PVOID *)(DAT_005b2584 * 4 + 0x5e4ea4) = pvVar2;
    return;
  }
  iVar3 = ___acrt_get_sigabrt_handler();
  if (iVar3 != 0) {
    FUN_005573ea(0x16);
  }
  if ((DAT_005b2650 & 2) != 0) {
    BVar4 = IsProcessorFeaturePresent(0x17);
    if (BVar4 != 0) {
      pcVar1 = (code *)swi(0x29);
      (*pcVar1)();
    }
    ___acrt_call_reportfault(3,0x40000015,1);
  }
                    /* WARNING: Subroutine does not return */
  __exit(3);
}

