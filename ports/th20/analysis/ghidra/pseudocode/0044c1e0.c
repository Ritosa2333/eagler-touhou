/* Entry: 0x0044c1e0; symbol: FUN_0044c1e0; body bytes: 84 */

void __thiscall FUN_0044c1e0(void *this,undefined4 param_1,undefined4 param_2)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
                    /* WARNING: Load size is inaccurate */
  FUN_00415520(*this,param_1,param_2,1);
  *unaff_FS_OFFSET = local_10;
  return;
}

