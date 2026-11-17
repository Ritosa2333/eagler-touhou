// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 4f11a0

void __thiscall FUN_004f11a0(int param_1,int param_2,undefined2 param_3)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 extraout_ECX;
  int iVar3;
  
  iVar3 = param_2 * 0x158;
  *(short *)(param_1 + 0x1078) = (short)param_2;
  *(undefined2 *)(param_1 + 0x107a) = param_3;
  uVar1 = *(undefined4 *)(&DAT_005c6794 + iVar3);
  *(undefined4 *)(param_1 + 0x678) = uVar1;
  *(undefined4 *)(param_1 + 0x674) = uVar1;
  *(undefined4 *)(param_1 + 0x6c0) = *(undefined4 *)(&DAT_005c6798 + iVar3);
  FUN_00404cf0();
  *(undefined4 *)(param_1 + 0x628) = 1;
  *(int *)(param_1 + 0x638) = param_1;
  FUN_004c19c0(extraout_ECX,*(undefined4 *)(&DAT_005c6650 + iVar3));
  *(uint *)(param_1 + 0x10) = *(uint *)(param_1 + 0x10) | 0x10;
  *(undefined1 *)(param_1 + 0x4e5) = 1;
  iVar2 = *(int *)(&DAT_005c67a4 + iVar3);
  if (iVar2 != 0) {
    FUN_004cb960(&param_2,iVar2,iVar2,param_1 + 0x650,0,0xffffffff,0,0);
    *(int *)(param_1 + 0x64c) = param_2;
  }
  switch(*(undefined4 *)(&DAT_005c679c + iVar3)) {
  case 0:
    *(int *)(param_1 + 0x690) = *(short *)(param_1 + 0x107a) * 2 + 6;
    return;
  case 1:
    *(undefined4 *)(param_1 + 0x690) =
         *(undefined4 *)(&DAT_00598c20 + *(short *)(param_1 + 0x107a) * 4);
    return;
  case 2:
    *(uint *)(param_1 + 0x10) = *(uint *)(param_1 + 0x10) | 0x10;
    *(undefined4 *)(param_1 + 0x690) = 0xffffffff;
    return;
  case 3:
    *(undefined4 *)(param_1 + 0x690) = 0x12;
    return;
  case 4:
    *(undefined4 *)(param_1 + 0x690) = 8;
    return;
  case 5:
    *(undefined4 *)(param_1 + 0x690) = 0xe;
    return;
  case 6:
    uVar1 = *(undefined4 *)
             (&DAT_005c6660 +
             *(short *)(param_1 + 0x107a) * 0x14 + *(short *)(param_1 + 0x1078) * 0x158);
    *(uint *)(param_1 + 0x10) = *(uint *)(param_1 + 0x10) | 0x10;
    *(undefined4 *)(param_1 + 0x690) = uVar1;
  }
  return;
}


// Reference 004fc0b4
// Address 40e0d0

undefined4 __fastcall FUN_0040e0d0(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 0x8156f8);
  if (*(int *)(param_1 + 0x8156e8 + iVar2 * 4) != 0) {
    iVar2 = iVar2 + 1;
    *(int *)(param_1 + 0x8156f8) = iVar2;
    if (3 < iVar2) {
      *(undefined4 *)(param_1 + 0x8156f8) = 3;
      iVar2 = 3;
    }
  }
  puVar1 = *(undefined4 **)(param_1 + 0x8156d8);
  param_1 = param_1 + iVar2 * 4;
  if (puVar1 != (undefined4 *)0x0) {
    *(undefined4 *)(param_1 + 0x8156e8) = puVar1[1];
    return *puVar1;
  }
  *(undefined4 *)(param_1 + 0x8156e8) = 0;
  return 0;
}


// Reference 004ee784
// Reference 004ee654
// Reference 004ee514
// Reference 004ee3d4
// Reference 004ee894
// Reference 004f4c4b
// Reference 004f9b06
// Reference 0051c92f
// Reference 004fbffb
