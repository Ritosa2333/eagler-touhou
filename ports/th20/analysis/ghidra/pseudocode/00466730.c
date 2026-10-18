/* Entry: 0x00466730; symbol: FUN_00466730; body bytes: 303 */

undefined4 __thiscall FUN_00466730(void *this,undefined4 *param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  char cVar4;
  float10 fVar5;
  int local_c;
  
  puVar1 = (undefined4 *)FUN_0045d670((void *)((int)this + 8),0);
  *puVar1 = 0;
  iVar2 = FUN_0045d670((void *)((int)this + 8),0);
  *(undefined4 *)(iVar2 + 4) = 0;
  fVar5 = FUN_004298b0(0x5ba4c4,DAT_0056e0f0);
  *(float *)((int)this + 0x170) = (float)fVar5;
  FUN_00423520((void *)((int)this + 0x174),1);
  cVar4 = '\0';
  for (local_c = 0; local_c < 0x1e; local_c = local_c + 1) {
    uVar3 = FUN_0040c300(param_1 + 8);
    puVar1 = (undefined4 *)FUN_00414580((void *)((int)this + 0xf8),local_c);
    *puVar1 = uVar3;
    if (0xe < local_c) {
      iVar2 = FUN_00414580((void *)((int)this + 0xf8),local_c);
      *(char *)(iVar2 + 3) = cVar4 * -8 + -1;
      cVar4 = cVar4 + '\x01';
    }
  }
  FUN_0045dd60(*(void **)((int)this + 4),param_1);
  FUN_00450350(*(void **)((int)this + 4),5);
  FUN_0045e590(*(int *)((int)this + 4));
  return 0;
}

