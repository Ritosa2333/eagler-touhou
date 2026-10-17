/* Entry: 0x005428cf; symbol: FUN_005428cf; body bytes: 129 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

uint __cdecl FUN_005428cf(int param_1)

{
  uint uVar1;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  uVar1 = 0x5a4d;
  if (((IMAGE_DOS_HEADER_00400000.e_magic == (char  [2])0x5a4d) &&
      (uVar1 = IMAGE_DOS_HEADER_00400000.e_lfanew,
      *(int *)(IMAGE_DOS_HEADER_00400000.e_lfanew + 0x400000) == 0x4550)) &&
     (*(short *)((int)IMAGE_DOS_HEADER_00400000.e_res_4_ + (IMAGE_DOS_HEADER_00400000.e_lfanew - 4))
      == 0x10b)) {
    uVar1 = FUN_0054279b(0x400000,param_1 - 0x400000);
    if ((uVar1 != 0) && (-1 < *(int *)(uVar1 + 0x24))) {
      uVar1 = CONCAT31((int3)(uVar1 >> 8),1);
      goto LAB_00542956;
    }
  }
  uVar1 = uVar1 & 0xffffff00;
LAB_00542956:
  *unaff_FS_OFFSET = local_14;
  return uVar1;
}

