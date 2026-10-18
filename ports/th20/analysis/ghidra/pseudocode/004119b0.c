/* Entry: 0x004119b0; symbol: FUN_004119b0; body bytes: 172 */

int * __thiscall FUN_004119b0(void *this,int param_1,int param_2)

{
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005674f0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *(int *)this = param_1;
  *(int *)((int)this + 4) = param_2;
                    /* WARNING: Load size is inaccurate */
  if (*this != 0) {
                    /* WARNING: Load size is inaccurate */
    FUN_00412da0(*this,this);
  }
  if (param_1 == 0) {
    local_18 = 0;
  }
  else {
    local_18 = FUN_0040ff90(param_1);
  }
  *(undefined4 *)((int)this + 4) = local_18;
  if (*(int *)((int)this + 4) != 0) {
    FUN_00412da0(*(void **)((int)this + 4),this);
  }
  *unaff_FS_OFFSET = local_10;
  return (int *)this;
}

