/* Entry: 0x005653d9; symbol: FUN_005653d9; body bytes: 212 */

WCHAR FUN_005653d9(WCHAR param_1,int *param_2)

{
  byte bVar1;
  int iVar2;
  int local_18;
  void *local_14;
  char local_c;
  WCHAR local_8 [2];
  
  if (param_1 == L'\xffff') {
    return 0xffff;
  }
  FUN_0054c44c(&local_18,param_2);
  bVar1 = (byte)param_1;
  if (*(int *)((int)local_14 + 8) == 0xfde9) {
    if ((ushort)param_1 < 0x80) {
      if (((&DAT_00599142)[(uint)bVar1 * 2] & 1) != 0) {
        bVar1 = *(byte *)(*(int *)((int)local_14 + 0x94) + (uint)bVar1);
      }
      param_1 = (WCHAR)bVar1;
    }
    else {
LAB_0056546e:
      iVar2 = ___acrt_LCMapStringW
                        (local_14,*(ushort **)((int)local_14 + 0xa8),0x100,&param_1,(void *)0x1,
                         local_8,1);
      if (iVar2 != 0) goto LAB_0056549a;
    }
  }
  else if ((ushort)param_1 < 0x100) {
    if (((&DAT_00599142)[(uint)bVar1 * 2] & 1) == 0) {
      param_1 = param_1 & 0xff;
    }
    else {
      param_1 = (WCHAR)*(byte *)(*(int *)((int)local_14 + 0x94) + (uint)bVar1);
    }
  }
  else if (*(int *)((int)local_14 + 0xa8) != 0) goto LAB_0056546e;
  local_8[0] = param_1;
LAB_0056549a:
  if (local_c != '\0') {
    *(uint *)(local_18 + 0x350) = *(uint *)(local_18 + 0x350) & 0xfffffffd;
  }
  return local_8[0];
}

