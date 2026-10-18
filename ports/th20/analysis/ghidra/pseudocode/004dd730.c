/* Entry: 0x004dd730; symbol: FUN_004dd730; body bytes: 234 */

void FUN_004dd730(void)

{
  FUN_004be220(&DAT_005ba568,0);
  FUN_004bdd60(&DAT_005ba568,0);
  if ((DAT_005c5888 >> 5 & 3) == 0) {
    DAT_005c5888 = DAT_005c5888 & 0xffffff9f | 0x20;
  }
  FUN_004217c0(DAT_005c4d28);
  FUN_004217c0(DAT_005c60b8);
  FUN_004bd3d0(DAT_005ba828);
  FUN_004217c0(DAT_005c6124);
  FUN_004217c0(DAT_005c4d2c);
  FUN_004217c0(DAT_005c49e8);
  FUN_004217c0(DAT_005c60fc);
  FUN_0049deb0(0);
  FUN_00534110(0);
  FUN_004217c0(DAT_005c4d24);
  FUN_004217c0(DAT_005c5b38);
  FUN_004217c0(DAT_005c6114);
  return;
}

