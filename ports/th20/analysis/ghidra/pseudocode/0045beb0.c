/* Entry: 0x0045beb0; symbol: FUN_0045beb0; body bytes: 475 */

void __thiscall FUN_0045beb0(void *this,double param_1)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  int *unaff_FS_OFFSET;
  uint local_28;
  uint local_20;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056828d;
  local_10 = *unaff_FS_OFFSET;
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = uVar2;
  uVar3 = FUN_0040c310(&DAT_005c0240,0xc);
  FUN_0040b730(&local_18,uVar3);
  local_8 = 0;
  (**(code **)(*(int *)**(undefined4 **)((int)this + 8) + 0x48))
            (**(undefined4 **)((int)this + 8),uVar2);
  iVar1 = *(int *)(*(int *)(*(int *)((int)this + 0x10) + 0x90) + 0x24);
  local_20 = (uint)*(ushort *)(*(int *)(*(int *)((int)this + 0x10) + 0x90) + 0x22) *
             ((int)(uint)*(ushort *)(*(int *)(*(int *)((int)this + 0x10) + 0x90) + 0x2e) >> 3) *
             (int)(((double)iVar1 + *(double *)(&DAT_0056cdd0 + (iVar1 >> 0x1f) * -8)) * param_1) -
             (uint)*(ushort *)(*(int *)(*(int *)((int)this + 0x10) + 0x90) + 0x2c);
  if ((int)local_20 < 0) {
    local_20 = 0;
  }
  FUN_0045b150(this,*(int *)(*(int *)((int)this + 0x10) + 0x90));
                    /* WARNING: Load size is inaccurate */
  (**(code **)(*this + 4))(local_20);
  *(undefined4 *)(*(int *)((int)this + 0x10) + 0x2c) =
       *(undefined4 *)(*(int *)((int)this + 0x10) + 8);
  local_28 = (uint)(*(int *)(*(int *)(*(int *)((int)this + 0x10) + 0x90) + 0x1c) != 0);
  FUN_0045a240(this,(int *)**(undefined4 **)((int)this + 8),local_28,local_20);
  FUN_0045add0(this,*(undefined4 *)((int)this + 0x24),*(undefined4 *)((int)this + 0x28),local_20);
  *(double *)((int)this + 0x38) = *(double *)((int)this + 0x38) - param_1;
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

