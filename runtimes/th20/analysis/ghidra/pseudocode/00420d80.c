/* Entry: 0x00420d80; symbol: FUN_00420d80; body bytes: 465 */

void __fastcall FUN_00420d80(CDocObjectServer *param_1)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056808d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004202c0(param_1);
  uVar1 = FUN_0040c310(&DAT_005c0240,0xf);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  *(undefined4 *)(param_1 + 0x14) = 0;
  FUN_00421ab0(param_1 + *(int *)(param_1 + 0x14) * 0x3d4 + 0x20,*(undefined4 *)(param_1 + 0x14));
  *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + 1;
  FUN_00420aa0(param_1);
  FUN_004210d0((int)param_1);
  std::_Adl_verify_range<char*,char_const*>((char **)"Pad Num : %d\n",*(char ***)(param_1 + 0x14));
  *(undefined4 *)(param_1 + 0x2e24) = DAT_005c4f70;
  *(undefined4 *)(param_1 + 0x2e28) = 0xffffffff;
  if (*(int *)(param_1 + 0x14) <= *(int *)(param_1 + 0x2e24)) {
    *(undefined4 *)(param_1 + 0x2e24) = 0;
  }
  if (*(int *)(param_1 + 0x2e24) < 1) {
    if (*(int *)(param_1 + 0x2e28) == 1) {
      if (2 < *(int *)(param_1 + 0x14)) {
        *(undefined4 *)(param_1 + 0x2e24) = 2;
      }
    }
    else if (1 < *(int *)(param_1 + 0x14)) {
      *(undefined4 *)(param_1 + 0x2e24) = 1;
    }
  }
  DAT_005c4f70 = *(undefined4 *)(param_1 + 0x2e24);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

