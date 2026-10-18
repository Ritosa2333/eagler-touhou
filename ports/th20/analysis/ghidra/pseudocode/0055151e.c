/* Entry: 0x0055151e; symbol: FUN_0055151e; body bytes: 105 */

/* WARNING: Removing unreachable block (ram,0x0055155f) */

undefined8 __cdecl FUN_0055151e(longlong *param_1)

{
  undefined4 uVar1;
  longlong lVar2;
  _FILETIME local_c;
  
  local_c.dwLowDateTime = 0;
  local_c.dwHighDateTime = 0;
  GetSystemTimeAsFileTime(&local_c);
  lVar2 = __alldiv(local_c.dwLowDateTime + 0x2ac18000,
                   (local_c.dwHighDateTime + 0xfe624e22) -
                   (uint)(local_c.dwLowDateTime < 0xd53e8000),10000000,0);
  uVar1 = (undefined4)((ulonglong)lVar2 >> 0x20);
  if (0x793582aff < lVar2) {
    lVar2 = -1;
    uVar1 = 0xffffffff;
  }
  if (param_1 != (longlong *)0x0) {
    *param_1 = lVar2;
  }
  return CONCAT44(uVar1,(int)lVar2);
}

