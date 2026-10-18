/* Entry: 0x0055c4c9; symbol: FUN_0055c4c9; body bytes: 119 */

short * __cdecl FUN_0055c4c9(short *param_1)

{
  int iVar1;
  uint local_c;
  uint local_8;
  
  iVar1 = FUN_005643e8(&local_8,param_1,0x100,L"TZ");
  if (iVar1 != 0) {
    if (iVar1 == 0x22) {
      param_1 = (short *)FUN_005584c0(local_8 * 2);
      if ((param_1 == (short *)0x0) ||
         (iVar1 = FUN_005643e8(&local_c,param_1,local_8,L"TZ"), iVar1 != 0)) {
        FUN_00557ba0(param_1);
        param_1 = (short *)0x0;
      }
      else {
        FUN_00557ba0((LPVOID)0x0);
      }
    }
    else {
      param_1 = (short *)0x0;
    }
  }
  return param_1;
}

