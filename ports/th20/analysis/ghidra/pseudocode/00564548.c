/* Entry: 0x00564548; symbol: FUN_00564548; body bytes: 147 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

undefined4 FUN_00564548(uint *param_1,undefined4 *param_2)

{
  uint uVar1;
  HANDLE hFile;
  BOOL BVar2;
  DWORD DVar3;
  __acrt_ptd *p_Var4;
  undefined4 uVar5;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  uVar5 = 0;
  FUN_0055fab5(*param_1);
  uVar1 = *(uint *)*param_2;
  if ((*(byte *)((&DAT_005e5690)[(int)uVar1 >> 6] + 0x28 + (uVar1 & 0x3f) * 0x38) & 1) != 0) {
    hFile = (HANDLE)FUN_0055fb8c(uVar1);
    BVar2 = FlushFileBuffers(hFile);
    if (BVar2 != 0) goto LAB_005645b8;
    DVar3 = GetLastError();
    p_Var4 = FUN_005516ac();
    *(DWORD *)p_Var4 = DVar3;
  }
  p_Var4 = FUN_005516c1();
  *(undefined4 *)p_Var4 = 9;
  uVar5 = 0xffffffff;
LAB_005645b8:
  FUN_005645de();
  *unaff_FS_OFFSET = local_14;
  return uVar5;
}

