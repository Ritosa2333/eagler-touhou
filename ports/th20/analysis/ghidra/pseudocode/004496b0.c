/* Entry: 0x004496b0; symbol: FUN_004496b0; body bytes: 170 */

LPVOID __thiscall FUN_004496b0(void *this,uint param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if ((param_1 & 2) == 0) {
    FUN_00449370((int)this);
    if ((param_1 & 1) != 0) {
      FUN_0054278d(this);
    }
  }
  else {
    FUN_00542f23(this,0x5e4,*(int *)((int)this + -4),FUN_00449370);
    if ((param_1 & 1) != 0) {
      FUN_0054303b((LPVOID)((int)this + -4));
    }
    this = (void *)((int)this + -4);
  }
  *unaff_FS_OFFSET = local_10;
  return this;
}

