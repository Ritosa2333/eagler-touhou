/* Entry: 0x004202c0; symbol: FUN_004202c0; body bytes: 466 */

void __fastcall FUN_004202c0(CDocObjectServer *param_1)

{
  char **ppcVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  int local_24;
  int local_20;
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005676ed;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = ppcVar1;
  uVar2 = FUN_0040c310(&DAT_005c0240,0xf);
  FUN_0040b730(&local_18,uVar2);
  local_8 = 0;
  *(undefined4 *)(param_1 + 0x14) = 0;
  for (local_24 = 0; local_24 < 0xc; local_24 = local_24 + 1) {
    FUN_00421720((undefined4 *)(param_1 + local_24 * 0x3d4 + 0x20));
  }
  if (*(int *)(param_1 + 0x2e10) != 0) {
    std::_Adl_verify_range<char*,char_const*>((char **)"DirectInput Release\n",ppcVar1);
    (**(code **)(**(int **)(param_1 + 0x2e10) + 0x20))(*(undefined4 *)(param_1 + 0x2e10));
    if (*(int *)(param_1 + 0x2e10) != 0) {
      (**(code **)(**(int **)(param_1 + 0x2e10) + 8))(*(undefined4 *)(param_1 + 0x2e10));
      *(undefined4 *)(param_1 + 0x2e10) = 0;
    }
  }
  for (local_20 = 0; local_20 < 4; local_20 = local_20 + 1) {
    if (*(int *)(param_1 + local_20 * 4 + 0x2e14) != 0) {
      std::_Adl_verify_range<char*,char_const*>((char **)"DirectInput(Pad) Release\n",ppcVar1);
      (**(code **)(**(int **)(param_1 + local_20 * 4 + 0x2e14) + 0x20))
                (*(undefined4 *)(param_1 + local_20 * 4 + 0x2e14));
      if (*(int *)(param_1 + local_20 * 4 + 0x2e14) != 0) {
        (**(code **)(**(int **)(param_1 + local_20 * 4 + 0x2e14) + 8))
                  (*(undefined4 *)(param_1 + local_20 * 4 + 0x2e14));
        *(undefined4 *)(param_1 + local_20 * 4 + 0x2e14) = 0;
      }
    }
  }
  std::_Adl_verify_range<char*,char_const*>((char **)"DirectInput Device Release\n",ppcVar1);
  CDocObjectServer::ReleaseDocSite(param_1);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

