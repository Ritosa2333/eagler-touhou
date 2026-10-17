/* Entry: 0x0043e9b0; symbol: FUN_0043e9b0; body bytes: 352 */

undefined4 __thiscall FUN_0043e9b0(void *this,undefined4 *param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  
  if (*(uint *)((int)this + 0x7c00e74) < (int)this + 0x7c00dccU) {
    puVar3 = param_1;
    puVar4 = *(undefined4 **)((int)this + 0x7c00e74);
    for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar4 = puVar4 + 1;
    }
    puVar3 = param_1 + 7;
    puVar4 = (undefined4 *)(*(int *)((int)this + 0x7c00e74) + 0x54);
    for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar4 = puVar4 + 1;
    }
    puVar3 = (undefined4 *)(*(int *)((int)this + 0x7c00e74) + 0x54);
    puVar4 = (undefined4 *)(*(int *)((int)this + 0x7c00e74) + 0x1c);
    for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar4 = puVar4 + 1;
    }
    puVar3 = param_1 + 0xe;
    puVar4 = (undefined4 *)(*(int *)((int)this + 0x7c00e74) + 0x70);
    for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar4 = puVar4 + 1;
    }
    puVar3 = (undefined4 *)(*(int *)((int)this + 0x7c00e74) + 0x70);
    puVar4 = (undefined4 *)(*(int *)((int)this + 0x7c00e74) + 0x38);
    for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar4 = puVar4 + 1;
    }
    puVar3 = param_1 + 0x15;
    puVar4 = (undefined4 *)(*(int *)((int)this + 0x7c00e74) + 0x8c);
    for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar4 = puVar4 + 1;
    }
    *(int *)((int)this + 0x7c00e74) = *(int *)((int)this + 0x7c00e74) + 0xa8;
    *(int *)((int)this + 0x6000e70) = *(int *)((int)this + 0x6000e70) + 1;
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

