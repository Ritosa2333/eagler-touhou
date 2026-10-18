/* Entry: 0x004fc550; symbol: FUN_004fc550; body bytes: 155 */

void __fastcall FUN_004fc550(int *param_1)

{
  uint uVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  int iVar4;
  undefined4 uVar5;
  byte *pbVar6;
  byte local_5;
  
  uVar1 = FUN_0040ff90((int)&stack0x00000004);
  if (4 < uVar1) {
    func_0x004fcd20("Invalid fill (too long).");
  }
  iVar4 = *param_1;
  puVar2 = (undefined1 *)FUN_004fcea0((int *)&stack0x00000004);
  puVar3 = (undefined1 *)FUN_0040c300((undefined4 *)&stack0x00000004);
  puVar2 = FUN_004e7be0(puVar3,puVar2,(undefined1 *)(iVar4 + 0xf));
  local_5 = 0;
  pbVar6 = &local_5;
  iVar4 = FUN_0040b520(*param_1 + 0xf);
  FUN_004f3350(puVar2,iVar4,pbVar6);
  uVar5 = FUN_0040ff90((int)&stack0x00000004);
  *(char *)(*param_1 + 0xe) = (char)uVar5;
  return;
}

