/* Entry: 0x0054bf64; symbol: FUN_0054bf64; body bytes: 108 */

uint __fastcall FUN_0054bf64(void *param_1)

{
  int *piVar1;
  int iVar2;
  uint3 uVar4;
  uint uVar3;
  undefined1 auStack_40 [28];
  undefined4 uStack_24;
  undefined8 local_14;
  char local_5;
  
  local_5 = '\0';
  local_14 = 0;
  piVar1 = *(int **)((int)param_1 + 0x50);
  FUN_0054a75b(auStack_40,(int)param_1 + 8,*(undefined4 *)((int)param_1 + 0x38),
               *(undefined4 *)((int)param_1 + 0x3c),&local_5);
  iVar2 = FUN_0054a803(piVar1);
  uVar4 = (uint3)((uint)iVar2 >> 8);
  if ((local_5 == '\0') || (iVar2 == 1)) {
    uVar3 = (uint)uVar4 << 8;
  }
  else if (*(char *)((int)param_1 + 0x30) == '\0') {
    uStack_24 = 0x54bfc8;
    uVar3 = FUN_0054c321(param_1,(undefined4 *)&local_14);
  }
  else {
    uVar3 = CONCAT31(uVar4,1);
  }
  return uVar3;
}

