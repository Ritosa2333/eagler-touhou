/* Entry: 0x00481780; symbol: FUN_00481780; body bytes: 337 */

undefined4 __thiscall FUN_00481780(void *this,int *param_1)

{
  int iVar1;
  int *unaff_FS_OFFSET;
  float10 fVar2;
  undefined4 in_stack_ffffffac;
  undefined4 in_stack_ffffffb0;
  uint uVar3;
  uint uVar4;
  float fVar5;
  uint local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_0056915d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  iVar1 = FUN_0040ff90(*(int *)((int)this + 0x286da0));
  fVar2 = FUN_004ff350(iVar1,(float *)(param_1 + 2));
  for (local_14 = 0; (int)local_14 < (int)*(short *)((int)param_1 + 0x26); local_14 = local_14 + 1)
  {
    for (local_18 = 0; (int)local_18 < (int)(short)param_1[9]; local_18 = local_18 + 1) {
      uVar3 = local_18;
      uVar4 = local_14;
      fVar5 = (float)fVar2;
      FUN_0047b850(&stack0xffffffac,(undefined4 *)&stack0x00000008);
      iVar1 = FUN_004818e0(this,param_1,in_stack_ffffffac,in_stack_ffffffb0,uVar3,uVar4,fVar5);
      if ((iVar1 != 0) && (iVar1 == 1)) goto LAB_00481866;
    }
  }
LAB_00481866:
  iVar1 = FUN_0040c300((undefined4 *)&stack0x00000008);
  if (-1 < *(int *)(iVar1 + 0x3c)) {
    fVar5 = (float)param_1[2];
    iVar1 = FUN_0040c300((undefined4 *)&stack0x00000008);
    FUN_00426eb0(&DAT_005ba830,*(int *)(iVar1 + 0x3c),fVar5);
  }
  local_8 = 0xffffffff;
  FUN_0047c300((int)&stack0x00000008);
  *unaff_FS_OFFSET = local_10;
  return 0;
}

