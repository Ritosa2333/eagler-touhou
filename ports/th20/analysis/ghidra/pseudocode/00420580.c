/* Entry: 0x00420580; symbol: FUN_00420580; body bytes: 471 */

void FUN_00420580(void)

{
  ushort uVar1;
  int iVar2;
  int local_24;
  uint local_20;
  undefined1 local_18 [4];
  ushort local_14;
  short local_10;
  short local_e;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  DAT_005b8e34 = DAT_005b8e30;
  FUN_00420760(0);
  FUN_00420760(1);
  local_20 = 0;
  for (local_24 = 0; local_24 < 4; local_24 = local_24 + 1) {
    iVar2 = XInputGetState(local_24,local_18);
    if (iVar2 == 0) {
      uVar1 = local_14;
      if (0x1ea8 < local_e) {
        uVar1 = local_14 | 1;
      }
      if (local_e < -0x1ea8) {
        uVar1 = uVar1 | 2;
      }
      if (0x1ea8 < local_10) {
        uVar1 = uVar1 | 8;
      }
      if (local_10 < -0x1ea8) {
        uVar1 = uVar1 | 4;
      }
      if ((uVar1 & 1) != 0) {
        local_20 = local_20 | 0x10;
      }
      if ((uVar1 & 2) != 0) {
        local_20 = local_20 | 0x20;
      }
      if ((uVar1 & 4) != 0) {
        local_20 = local_20 | 0x40;
      }
      if ((uVar1 & 8) != 0) {
        local_20 = local_20 | 0x80;
      }
      if (((((uVar1 & 0x1000) != 0) || ((uVar1 & 0x2000) != 0)) || ((uVar1 & 0x4000) != 0)) ||
         ((uVar1 & 0x8000) != 0)) {
        local_20 = local_20 | 1;
      }
    }
  }
  DAT_005b8e30 = DAT_005b88b0 | DAT_005b8b70 | local_20;
  FUN_004228b0(&DAT_005b8e30);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

