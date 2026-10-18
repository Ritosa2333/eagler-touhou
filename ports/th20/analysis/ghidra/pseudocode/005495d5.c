/* Entry: 0x005495d5; symbol: FUN_005495d5; body bytes: 120 */

undefined4 __thiscall FUN_005495d5(void *this,uint *param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined1 uVar5;
  char *local_8;
  
  uVar5 = 1;
  piVar1 = *(int **)((int)this + 8);
  local_8 = (char *)(*(int *)((int)this + 0xc) + -1);
  iVar2 = piVar1[6];
  iVar3 = piVar1[7];
  uVar4 = FUN_00548a66(piVar1,local_8,&local_8,10,1);
  *param_1 = uVar4;
  if (((*(char *)(*(int *)((int)this + 8) + 0x1c) == '\0') ||
      (*(int *)(*(int *)((int)this + 8) + 0x18) != 0x22)) &&
     (*(char **)((int)this + 0xc) <= local_8)) {
    *(char **)((int)this + 0xc) = local_8;
  }
  else {
    uVar5 = 0;
  }
  piVar1[6] = iVar2;
  piVar1[7] = iVar3;
  return CONCAT31((int3)((uint)iVar3 >> 8),uVar5);
}

