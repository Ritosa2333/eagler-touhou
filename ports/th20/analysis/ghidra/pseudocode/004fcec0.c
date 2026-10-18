/* Entry: 0x004fcec0; symbol: FUN_004fcec0; body bytes: 125 */

undefined4 __thiscall FUN_004fcec0(void *this,byte *param_1,byte *param_2)

{
  int iVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  iVar1 = *(int *)((int)this + 4);
  if (iVar1 != 1) {
    if (iVar1 == 2) {
      uVar2 = FUN_004fbdb0(this,(char *)param_1,(int)param_2);
      goto LAB_004fcf2c;
    }
    if (iVar1 == 4) {
      uVar2 = FUN_004fcfa0(param_1,param_2);
      goto LAB_004fcf2c;
    }
  }
  uVar2 = 1;
LAB_004fcf2c:
  *unaff_FS_OFFSET = local_10;
  return uVar2;
}

