/* Entry: 0x0040cfc0; symbol: FUN_0040cfc0; body bytes: 116 */

void __cdecl FUN_0040cfc0(LPVOID param_1,uint param_2)

{
  char cVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005676c0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    if (0xfff < param_2) {
      FUN_0040ec20((int *)&param_1,(int *)&param_2);
    }
    FUN_0054278d(param_1);
  }
  else {
    thunk_FUN_00557ba0(param_1);
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

