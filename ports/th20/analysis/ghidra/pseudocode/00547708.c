/* Entry: 0x00547708; symbol: ___vcrt_uninitialize_locks; body bytes: 47 */

/* Library Function - Single Match
    ___vcrt_uninitialize_locks
   
   Library: Visual Studio 2019 Release */

undefined4 ___vcrt_uninitialize_locks(void)

{
  undefined4 in_EAX;
  undefined4 extraout_EAX;
  int iVar1;
  LPCRITICAL_SECTION lpCriticalSection;
  
  if (DAT_005e52e8 != 0) {
    lpCriticalSection = (LPCRITICAL_SECTION)(&DAT_005e52b8 + DAT_005e52e8 * 0x18);
    iVar1 = DAT_005e52e8;
    do {
      DeleteCriticalSection(lpCriticalSection);
      DAT_005e52e8 = DAT_005e52e8 + -1;
      lpCriticalSection = lpCriticalSection + -1;
      iVar1 = iVar1 + -1;
      in_EAX = extraout_EAX;
    } while (iVar1 != 0);
  }
  return CONCAT31((int3)((uint)in_EAX >> 8),1);
}

