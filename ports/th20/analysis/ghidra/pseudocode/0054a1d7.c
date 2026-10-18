/* Entry: 0x0054a1d7; symbol: FUN_0054a1d7; body bytes: 133 */

uint __fastcall FUN_0054a1d7(int param_1)

{
  undefined4 *puVar1;
  uint *puVar2;
  int iVar3;
  bool bVar4;
  undefined3 extraout_var;
  uint uVar5;
  
  puVar1 = *(undefined4 **)(param_1 + 0x10);
  *(undefined4 **)(param_1 + 0x10) = puVar1 + 1;
  puVar2 = (uint *)*puVar1;
  bVar4 = FUN_00559818();
  if (CONCAT31(extraout_var,bVar4) == 0) {
LAB_0054a1f1:
    iVar3 = *(int *)(param_1 + 8);
    *(undefined1 *)(iVar3 + 0x1c) = 1;
    *(undefined4 *)(iVar3 + 0x18) = 0x16;
    uVar5 = FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,*(int **)(param_1 + 8));
    uVar5 = uVar5 & 0xffffff00;
  }
  else {
    uVar5 = __crt_stdio_output::to_integer_size(*(length_modifier *)(param_1 + 0x28));
    if (uVar5 == 1) {
      uVar5 = 0;
      *(undefined1 *)puVar2 = *(undefined1 *)(param_1 + 0x14);
    }
    else if (uVar5 == 2) {
      uVar5 = (uint)*(ushort *)(param_1 + 0x14);
      *(ushort *)puVar2 = *(ushort *)(param_1 + 0x14);
    }
    else if (uVar5 == 4) {
      uVar5 = *(uint *)(param_1 + 0x14);
      *puVar2 = uVar5;
    }
    else {
      if (uVar5 != 8) goto LAB_0054a1f1;
      uVar5 = *(uint *)(param_1 + 0x14);
      *puVar2 = uVar5;
      puVar2[1] = (int)uVar5 >> 0x1f;
    }
    *(undefined1 *)(param_1 + 0x2c) = 1;
    uVar5 = CONCAT31((int3)(uVar5 >> 8),1);
  }
  return uVar5;
}

