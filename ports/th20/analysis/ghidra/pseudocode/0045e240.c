/* Entry: 0x0045e240; symbol: FUN_0045e240; body bytes: 340 */

undefined4 __thiscall FUN_0045e240(void *this,undefined4 *param_1)

{
  char cVar1;
  undefined4 *puVar2;
  int iVar3;
  char *pcVar4;
  float10 fVar5;
  undefined4 local_c;
  
  puVar2 = (undefined4 *)FUN_0045d670((void *)((int)this + 8),0);
  *puVar2 = 0;
  iVar3 = FUN_0045d670((void *)((int)this + 8),0);
  *(undefined4 *)(iVar3 + 4) = 0;
  fVar5 = FUN_004298b0(0x5ba4c4,DAT_0056e0f0);
  *(float *)((int)this + 0x308) = (float)fVar5;
  FUN_00423520((void *)((int)this + 0x30c),1);
  cVar1 = '\0';
  for (local_c = 0; local_c < 0x40; local_c = local_c + 1) {
    puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x208),local_c);
    *puVar2 = 0xff505050;
    if (local_c < 8) {
      pcVar4 = (char *)FUN_00414580((void *)((int)this + 0x208),local_c);
      *pcVar4 = -1 - (char)(local_c << 5);
    }
    if (0x1f < local_c) {
      iVar3 = FUN_00414580((void *)((int)this + 0x208),local_c);
      *(char *)(iVar3 + 3) = cVar1 * -0x10 + -1;
      cVar1 = cVar1 + '\x01';
    }
  }
  FUN_0045dd60(*(void **)((int)this + 4),param_1);
  FUN_00450350(*(void **)((int)this + 4),0x13);
  FUN_0045e590(*(int *)((int)this + 4));
  return 0;
}

