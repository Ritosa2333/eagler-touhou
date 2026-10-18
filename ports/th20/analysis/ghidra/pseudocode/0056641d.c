/* Entry: 0x0056641d; symbol: FUN_0056641d; body bytes: 130 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

undefined4 FUN_0056641d(uint *param_1,undefined4 *param_2)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  FUN_0055fab5(*param_1);
  uVar1 = *(uint *)*param_2;
  iVar2 = param_2[1];
  if ((*(byte *)((&DAT_005e5690)[(int)uVar1 >> 6] + 0x28 + (uVar1 & 0x3f) * 0x38) & 1) == 0) {
    *(undefined1 *)(iVar2 + 0x1c) = 1;
    *(undefined4 *)(iVar2 + 0x18) = 9;
    uVar3 = 0xffffffff;
  }
  else {
    uVar3 = FUN_00566551(uVar1,iVar2);
  }
  FUN_005664a2();
  *unaff_FS_OFFSET = local_14;
  return uVar3;
}

