/* Entry: 0x00551eba; symbol: FUN_00551eba; body bytes: 49 */

undefined4 FUN_00551eba(void)

{
  undefined4 in_EAX;
  undefined4 extraout_EAX;
  int iVar1;
  LPCRITICAL_SECTION lpCriticalSection;
  
  if (DAT_005e5488 != 0) {
    lpCriticalSection = (LPCRITICAL_SECTION)(&DAT_005e5308 + DAT_005e5488 * 0x18);
    iVar1 = DAT_005e5488;
    do {
      DeleteCriticalSection(lpCriticalSection);
      DAT_005e5488 = DAT_005e5488 + -1;
      lpCriticalSection = lpCriticalSection + -1;
      iVar1 = iVar1 + -1;
      in_EAX = extraout_EAX;
    } while (iVar1 != 0);
  }
  return CONCAT31((int3)((uint)in_EAX >> 8),1);
}

