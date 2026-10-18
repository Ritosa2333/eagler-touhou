/* Entry: 0x005572d3; symbol: FUN_005572d3; body bytes: 89 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

uint FUN_005572d3(int *param_1)

{
  byte bVar1;
  uint uVar2;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(*param_1);
  bVar1 = (byte)DAT_005b25c0 & 0x1f;
  uVar2 = DAT_005e55f0 ^ DAT_005b25c0;
  FUN_0055732f();
  *unaff_FS_OFFSET = local_14;
  return uVar2 >> bVar1 | uVar2 << 0x20 - bVar1;
}

