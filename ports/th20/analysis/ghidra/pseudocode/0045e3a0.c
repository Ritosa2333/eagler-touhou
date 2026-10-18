/* Entry: 0x0045e3a0; symbol: FUN_0045e3a0; body bytes: 382 */

undefined4 __thiscall FUN_0045e3a0(void *this,undefined4 *param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  int iVar2;
  char cVar3;
  float10 fVar4;
  undefined4 local_c;
  
  *(undefined4 *)((int)this + 0x31c) = param_2;
  puVar1 = (undefined4 *)FUN_0045d670((void *)((int)this + 8),0);
  *puVar1 = 0;
  iVar2 = FUN_0045d670((void *)((int)this + 8),0);
  *(undefined4 *)(iVar2 + 4) = 0;
  fVar4 = FUN_004298b0(0x5ba4c4,DAT_0056e0f0);
  *(float *)((int)this + 0x308) = (float)fVar4;
  FUN_00423520((void *)((int)this + 0x30c),1);
  cVar3 = '\0';
  for (local_c = 0; local_c < 0x40; local_c = local_c + 1) {
    puVar1 = (undefined4 *)FUN_00414580((void *)((int)this + 0x208),local_c);
    *puVar1 = 0xff0080ff;
    if (local_c < 8) {
      iVar2 = FUN_00414580((void *)((int)this + 0x208),local_c);
      *(char *)(iVar2 + 2) = -1 - (char)(local_c << 5);
    }
    if (0x1f < local_c) {
      iVar2 = FUN_00414580((void *)((int)this + 0x208),local_c);
      *(char *)(iVar2 + 3) = cVar3 * -0x10 + -1;
      cVar3 = cVar3 + '\x01';
    }
  }
  FUN_0045dd60(*(void **)((int)this + 4),param_1);
  FUN_00450350(*(void **)((int)this + 4),0xf);
  FUN_0045e5b0(*(int *)((int)this + 4));
  FUN_00438c60(*(void **)((int)this + 4),0x40,0,0xff,0);
  return 0;
}

