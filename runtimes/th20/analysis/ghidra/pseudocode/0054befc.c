/* Entry: 0x0054befc; symbol: FUN_0054befc; body bytes: 104 */

uint __fastcall FUN_0054befc(void *param_1)

{
  int *piVar1;
  int iVar2;
  uint3 uVar4;
  uint uVar3;
  undefined1 auStack_3c [28];
  undefined4 uStack_20;
  undefined4 local_c;
  char local_5;
  
  local_c = 0;
  local_5 = '\0';
  piVar1 = *(int **)((int)param_1 + 0x50);
  FUN_0054a75b(auStack_3c,(int)param_1 + 8,*(undefined4 *)((int)param_1 + 0x38),
               *(undefined4 *)((int)param_1 + 0x3c),&local_5);
  iVar2 = FUN_0054a779(piVar1);
  uVar4 = (uint3)((uint)iVar2 >> 8);
  if ((local_5 == '\0') || (iVar2 == 1)) {
    uVar3 = (uint)uVar4 << 8;
  }
  else if (*(char *)((int)param_1 + 0x30) == '\0') {
    uStack_20 = 0x54bf5c;
    uVar3 = FUN_0054c2ec(param_1,&local_c);
  }
  else {
    uVar3 = CONCAT31(uVar4,1);
  }
  return uVar3;
}

