/* Entry: 0x004712d0; symbol: FUN_004712d0; body bytes: 213 */

int * FUN_004712d0(void)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  void *this;
  undefined4 *puVar4;
  int *piVar5;
  int *unaff_FS_OFFSET;
  int *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568ef4;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = operator_new(0x24);
  local_8 = 0;
  if (this == (void *)0x0) {
    local_18 = (int *)0x0;
  }
  else {
    FUN_0040c080(this,0x24);
    puVar4 = (undefined4 *)move<>(&stack0x00000014);
    uVar1 = *puVar4;
    piVar5 = (int *)move<>(&stack0x00000010);
    iVar2 = *piVar5;
    piVar5 = (int *)move<>(&stack0x0000000c);
    iVar3 = *piVar5;
    piVar5 = (int *)move<>(&stack0x00000008);
    local_18 = FUN_0049cf80(this,*piVar5,iVar3,iVar2,uVar1);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

