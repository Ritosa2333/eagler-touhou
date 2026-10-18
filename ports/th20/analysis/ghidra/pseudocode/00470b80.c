/* Entry: 0x00470b80; symbol: FUN_00470b80; body bytes: 709 */

void __thiscall FUN_00470b80(void *this,int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  float fVar5;
  
  iVar1 = FUN_00437cd0((int)this);
  fVar5 = (float)param_1 / *(float *)(iVar1 + 0x20);
  *(float *)((int)this + 0x388) = fVar5;
  *(float *)((int)this + 0x378) = fVar5;
  fVar5 = (float)(param_1 + param_3) / *(float *)(iVar1 + 0x20);
  *(float *)((int)this + 0x390) = fVar5;
  *(float *)((int)this + 0x380) = fVar5;
  fVar5 = (float)param_2 / *(float *)(iVar1 + 0x1c);
  *(float *)((int)this + 900) = fVar5;
  *(float *)((int)this + 0x37c) = fVar5;
  fVar5 = (float)(param_2 + param_4) / *(float *)(iVar1 + 0x20);
  *(float *)((int)this + 0x394) = fVar5;
  *(float *)((int)this + 0x38c) = fVar5;
  *(float *)((int)this + 0x398) = (float)param_3 / *(float *)(iVar1 + 0x20);
  *(float *)((int)this + 0x39c) = (float)param_4 / *(float *)(iVar1 + 0x1c);
  *(float *)((int)this + 0x70) = (float)param_3;
  *(float *)((int)this + 0x74) = (float)param_4;
  puVar3 = &DAT_0056f308;
  puVar4 = (undefined4 *)((int)this + 0x3b8);
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  puVar3 = &DAT_0056f308;
  puVar4 = (undefined4 *)((int)this + 0x3f8);
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  *(float *)((int)this + 0x3b8) = *(float *)((int)this + 0x70) / DAT_0056cda0;
  *(float *)((int)this + 0x3cc) = *(float *)((int)this + 0x74) / DAT_0056cda0;
  *(float *)((int)this + 0x3f8) =
       (*(float *)((int)this + 0x70) / *(float *)(iVar1 + 0x20)) * *(float *)(iVar1 + 0x50);
  *(float *)((int)this + 0x40c) =
       (*(float *)((int)this + 0x74) / *(float *)(iVar1 + 0x1c)) * *(float *)(iVar1 + 0x54);
  puVar3 = (undefined4 *)((int)this + 0x3b8);
  puVar4 = (undefined4 *)((int)this + 0x57c);
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  *(int *)((int)this + 0x3a8) = param_1;
  *(int *)((int)this + 0x3ac) = param_2;
  *(int *)((int)this + 0x3b0) = param_1 + param_3;
  *(int *)((int)this + 0x3b4) = param_2 + param_4;
  return;
}

