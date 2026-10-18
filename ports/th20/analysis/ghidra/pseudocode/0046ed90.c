/* Entry: 0x0046ed90; symbol: FUN_0046ed90; body bytes: 329 */

void __thiscall FUN_0046ed90(void *this,int param_1)

{
  char cVar1;
  bool bVar2;
  uint *puVar3;
  uint *puVar4;
  char **ppcVar5;
  undefined1 local_6;
  undefined1 local_5;
  
  puVar3 = (uint *)((int)this + 4);
  ppcVar5 = (char **)(param_1 + 4);
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    puVar3 = (uint *)move<>((int)this + 4);
    puVar4 = (uint *)move<>(param_1 + 4);
    FUN_00543e20(puVar3,puVar4,0x18);
    *(undefined4 *)(param_1 + 0x14) = 0;
    *(undefined4 *)(param_1 + 0x18) = 0xf;
    FUN_0040eba0((int)ppcVar5);
    local_5 = 0;
    FUN_0040fe90((undefined1 *)ppcVar5,&local_5);
  }
  else {
    bVar2 = FUN_0040f180((int)ppcVar5);
    if (bVar2) {
      FUN_0046ed60();
      FUN_0040cd70(puVar3,ppcVar5);
      FUN_0040f6a0(ppcVar5);
    }
    else {
      FUN_0040e5e0();
      FUN_0040eba0((int)puVar3);
      FUN_0040ffd0(puVar3,(uint *)ppcVar5,*(int *)(param_1 + 0x14) + 1);
    }
    *(undefined4 *)((int)this + 0x18) = *(undefined4 *)(param_1 + 0x18);
    *(undefined4 *)((int)this + 0x14) = *(undefined4 *)(param_1 + 0x14);
    *(undefined4 *)(param_1 + 0x14) = 0;
    *(undefined4 *)(param_1 + 0x18) = 0xf;
    local_6 = 0;
    FUN_0040fe90((undefined1 *)ppcVar5,&local_6);
  }
  return;
}

