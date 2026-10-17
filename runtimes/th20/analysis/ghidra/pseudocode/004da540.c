/* Entry: 0x004da540; symbol: FUN_004da540; body bytes: 1402 */

void __fastcall FUN_004da540(int param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569d7d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  if (*(int *)(param_1 + 0xb08) == *(int *)(param_1 + 0xb0c)) goto LAB_004daaa0;
  uVar1 = FUN_0040c310(&DAT_005c0240,5);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  *(undefined4 *)(param_1 + 0xb10) = *(undefined4 *)(param_1 + 0xb08);
  std::_Adl_verify_range<char*,char_const*>
            ((char **)"scene %d -> %d\r\n",*(char ***)(param_1 + 0xb08));
  *(undefined4 *)(param_1 + 0xde0) = 0xff000000;
  switch(*(undefined4 *)(param_1 + 0xb0c)) {
  case 0:
    *(undefined4 *)(param_1 + 0xb0c) = 1;
    puVar2 = FUN_004d85c0();
    *(undefined4 **)(param_1 + 0xdc8) = puVar2;
    if (puVar2 != (undefined4 *)0x0) break;
    *(undefined4 *)(param_1 + 0xb0c) = 3;
  case 3:
    FUN_004dd730();
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
    goto LAB_004daaa0;
  case 4:
    switch(*(undefined4 *)(param_1 + 0xb08)) {
    case 1:
    case 2:
      FUN_0052cfe0();
      break;
    case 7:
      FUN_004bd3d0(DAT_005ba828);
      FUN_0052cfe0();
      break;
    case 0xf:
      FUN_004217c0(DAT_005c49e8);
      FUN_0052cfe0();
    }
    break;
  case 7:
    if (*(int *)(param_1 + 0xb08) == 4) {
      FUN_004217c0(DAT_005c6124);
    }
    *(undefined4 *)(param_1 + 0xb18) = 1;
    FUN_004bec70(0);
    break;
  case 10:
    FUN_004bd3d0(DAT_005ba828);
    *(undefined4 *)(param_1 + 0xb0c) = 7;
    *(undefined4 *)(param_1 + 0xb18) = 1;
    *(undefined4 *)(param_1 + 0xb1c) = 0;
    iVar3 = FUN_00498f40();
    FUN_004dd8d0(iVar3);
    FUN_004bec70(0);
    break;
  case 0xb:
    FUN_004bd3d0(DAT_005ba828);
    *(undefined4 *)(param_1 + 0xb18) = 1;
    *(undefined4 *)(param_1 + 0xb1c) = 0;
    *(undefined4 *)(param_1 + 0xb0c) = 7;
    iVar3 = FUN_00498f40();
    FUN_004dd8d0(iVar3);
    FUN_004bec70(1);
    break;
  case 0xc:
    uVar1 = FUN_00488830((int)DAT_005ba828);
    *(undefined4 *)(param_1 + 0xb18) = 0;
    if (*(int *)(param_1 + 0xb08) == 7) {
      FUN_004bd3d0(DAT_005ba828);
    }
    *(undefined4 *)(param_1 + 0xb0c) = 7;
    FUN_004bec70(uVar1);
    break;
  case 0xd:
    if (*(int *)(param_1 + 0xb08) == 4) {
      FUN_004217c0(DAT_005c6124);
    }
    *(undefined4 *)(param_1 + 0xb0c) = 7;
    *(undefined4 *)(param_1 + 0xb18) = 1;
    FUN_004bec70(1);
    break;
  case 0xe:
    FUN_004bd3d0(DAT_005ba828);
    *(undefined4 *)(param_1 + 0xb18) = 1;
    *(undefined4 *)(param_1 + 0xb0c) = 7;
    FUN_004bec70(0);
    break;
  case 0xf:
    if (*(int *)(param_1 + 0xb08) == 7) {
      FUN_004bd3d0(DAT_005ba828);
    }
    FUN_004a1010();
    break;
  case 0x10:
    iVar3 = *(int *)(param_1 + 0xb08);
    if (iVar3 == 2) {
LAB_004da705:
      *(undefined4 *)(param_1 + 0xb0c) = 4;
      DAT_005c6128 = 3;
      FUN_0052cfe0();
    }
    else if (iVar3 == 7) {
      FUN_004bd3d0(DAT_005ba828);
      *(undefined4 *)(param_1 + 0xb0c) = 4;
      DAT_005c6128 = 3;
      FUN_0052cfe0();
    }
    else if (iVar3 == 0xf) {
      FUN_004217c0(DAT_005c49e8);
      goto LAB_004da705;
    }
    break;
  case 0x11:
    FUN_004dd730();
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
    goto LAB_004daaa0;
  case 0x13:
    FUN_004bd3d0(DAT_005ba828);
    *(undefined4 *)(param_1 + 0xb18) = 1;
    *(undefined4 *)(param_1 + 0xb1c) = 1;
    *(undefined4 *)(param_1 + 0xb0c) = 7;
    iVar3 = FUN_00498f40();
    FUN_004dd8d0(iVar3);
    FUN_004bec70(0);
    break;
  case 0x16:
    FUN_004bd3d0(DAT_005ba828);
    *(undefined4 *)(param_1 + 0xb0c) = 7;
    iVar3 = FUN_00498f40();
    FUN_004dd8d0(iVar3);
    FUN_004bec70(0);
    break;
  case 0x18:
    iVar3 = FUN_00498f40();
    iVar3 = FUN_00474d80(iVar3);
    iVar4 = FUN_00498f40();
    iVar4 = FUN_004bd5c0(iVar4);
    if (iVar3 != iVar4) {
      *(undefined4 *)(param_1 + 0xb0c) = 7;
    }
    FUN_004bd3d0(DAT_005ba828);
    iVar3 = FUN_00498f40();
    iVar3 = FUN_00474d80(iVar3);
    iVar4 = FUN_00498f40();
    iVar4 = FUN_004bd5c0(iVar4);
    if (iVar3 == iVar4) {
      *(undefined4 *)(param_1 + 0xb0c) = 7;
    }
    *(undefined4 *)(param_1 + 0xb18) = 1;
    *(undefined4 *)(param_1 + 0xb1c) = 0;
    iVar3 = FUN_00498f40();
    FUN_004dd8d0(iVar3);
    FUN_004bec70(0);
  }
  *(undefined4 *)(param_1 + 0xb08) = *(undefined4 *)(param_1 + 0xb0c);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
LAB_004daaa0:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

