/* Entry: 0x00438620; symbol: FUN_00438620; body bytes: 716 */

undefined4 __thiscall FUN_00438620(void *this,int param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  
  if (*(int *)((int)this + 0x3c) == 0) {
    uVar1 = 0xffffffff;
  }
  else {
    *(int *)(param_1 + 0x20) = param_2;
    iVar2 = FUN_00437c80(this,param_2);
    *(undefined4 *)(param_1 + 0x388) = *(undefined4 *)(iVar2 + 0x38);
    *(undefined4 *)(param_1 + 0x378) = *(undefined4 *)(param_1 + 0x388);
    *(undefined4 *)(param_1 + 0x390) = *(undefined4 *)(iVar2 + 0x40);
    *(undefined4 *)(param_1 + 0x380) = *(undefined4 *)(param_1 + 0x390);
    *(undefined4 *)(param_1 + 900) = *(undefined4 *)(iVar2 + 0x3c);
    *(undefined4 *)(param_1 + 0x37c) = *(undefined4 *)(param_1 + 900);
    *(undefined4 *)(param_1 + 0x394) = *(undefined4 *)(iVar2 + 0x44);
    *(undefined4 *)(param_1 + 0x38c) = *(undefined4 *)(param_1 + 0x394);
    *(float *)(param_1 + 0x398) = *(float *)(iVar2 + 0x40) - *(float *)(iVar2 + 0x38);
    *(float *)(param_1 + 0x39c) = *(float *)(iVar2 + 0x44) - *(float *)(iVar2 + 0x3c);
    *(undefined4 *)(param_1 + 0x70) = *(undefined4 *)(iVar2 + 0x4c);
    *(undefined4 *)(param_1 + 0x74) = *(undefined4 *)(iVar2 + 0x48);
    puVar4 = &DAT_0056e068;
    puVar5 = (undefined4 *)(param_1 + 0x3b8);
    for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
    puVar4 = &DAT_0056e068;
    puVar5 = (undefined4 *)(param_1 + 0x3f8);
    for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
    *(float *)(param_1 + 0x3b8) = *(float *)(param_1 + 0x70) / DAT_0056cda0;
    *(float *)(param_1 + 0x3cc) = *(float *)(param_1 + 0x74) / DAT_0056cda0;
    *(float *)(param_1 + 0x3f8) =
         (*(float *)(param_1 + 0x70) / *(float *)(iVar2 + 0x20)) * *(float *)(iVar2 + 0x50);
    *(float *)(param_1 + 0x40c) =
         (*(float *)(param_1 + 0x74) / *(float *)(iVar2 + 0x1c)) * *(float *)(iVar2 + 0x54);
    puVar4 = (undefined4 *)(param_1 + 0x3b8);
    puVar5 = (undefined4 *)(param_1 + 0x57c);
    for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
    *(int *)(param_1 + 0x3a8) = (int)*(float *)(iVar2 + 0xc);
    *(int *)(param_1 + 0x3ac) = (int)*(float *)(iVar2 + 0x10);
    *(int *)(param_1 + 0x3b0) = (int)*(float *)(iVar2 + 0x14);
    *(int *)(param_1 + 0x3b4) = (int)*(float *)(iVar2 + 0x18);
    uVar1 = 0;
  }
  return uVar1;
}

