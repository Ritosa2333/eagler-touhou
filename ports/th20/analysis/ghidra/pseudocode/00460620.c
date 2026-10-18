/* Entry: 0x00460620; symbol: FUN_00460620; body bytes: 477 */

undefined4 __thiscall FUN_00460620(void *this,undefined4 *param_1,undefined4 param_2)

{
  undefined1 uVar1;
  undefined4 *puVar2;
  int iVar3;
  char *pcVar4;
  char cVar5;
  int local_10;
  
  *(undefined4 *)((int)this + 0x3e0) = param_2;
  puVar2 = (undefined4 *)FUN_0045d670((void *)((int)this + 8),0);
  *puVar2 = 0;
  iVar3 = FUN_0045d670((void *)((int)this + 8),0);
  *(undefined4 *)(iVar3 + 4) = 0;
  *(undefined4 *)((int)this + 0x3cc) = param_1[7];
  *(undefined4 *)((int)this + 0x3c8) = param_1[6];
  FUN_00423520((void *)((int)this + 0x3d0),1);
  cVar5 = '\0';
  for (local_10 = 0; local_10 < 0x50; local_10 = local_10 + 1) {
    iVar3 = FUN_00414580((void *)((int)this + 0x288),local_10);
    *(undefined1 *)(iVar3 + 2) = 0xff;
    pcVar4 = (char *)FUN_00414580((void *)((int)this + 0x288),local_10);
    *pcVar4 = ('P' - (char)local_10) * '\x03';
    uVar1 = FUN_00449d20(0x5ba4c4);
    iVar3 = FUN_00414580((void *)((int)this + 0x288),local_10);
    *(undefined1 *)(iVar3 + 1) = uVar1;
    iVar3 = FUN_00414580((void *)((int)this + 0x288),local_10);
    *(char *)(iVar3 + 3) = ('P' - (char)local_10) * '\x03';
    if (0x27 < local_10) {
      iVar3 = FUN_00414580((void *)((int)this + 0x288),local_10);
      *(char *)(iVar3 + 3) = cVar5 * -0x20 + -1;
      cVar5 = cVar5 + '\x01';
    }
    if (local_10 < 8) {
      iVar3 = FUN_00414580((void *)((int)this + 0x288),local_10);
      *(char *)(iVar3 + 3) = (char)(local_10 << 5);
    }
  }
  FUN_0045dd60(*(void **)((int)this + 4),param_1);
  FUN_00450350(*(void **)((int)this + 4),5);
  FUN_00438c60(*(void **)((int)this + 4),0x50,0,0xff,0);
  return 0;
}

