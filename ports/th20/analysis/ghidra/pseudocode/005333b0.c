/* Entry: 0x005333b0; symbol: FUN_005333b0; body bytes: 834 */

undefined4 FUN_005333b0(void)

{
  int iVar1;
  int iVar2;
  void *local_a8 [25];
  int local_44;
  void *local_40;
  int local_3c;
  void *local_38;
  void *local_34;
  void *local_30;
  void *local_2c;
  void *local_28;
  void *local_24;
  void *local_20;
  void *local_1c;
  void *local_18;
  void *local_14;
  void *local_10;
  void *local_c;
  int local_8;
  
  local_c = (void *)FUN_00464080(0);
  FUN_004bdd10(local_c,0);
  local_10 = (void *)FUN_00464080(0);
  FUN_004bec10(local_10,0);
  local_14 = (void *)FUN_00464080(0);
  FUN_004be990(local_14,0);
  local_18 = (void *)FUN_00464080(0);
  FUN_004be190(local_18,0);
  local_1c = (void *)FUN_00464080(0);
  FUN_004beaf0(local_1c,0);
  local_20 = (void *)FUN_00464080(0);
  FUN_004bdc40(local_20,0);
  (**(code **)**(undefined4 **)(local_8 + 0x28))();
  (**(code **)(**(int **)(local_8 + 0x28) + 4))();
  (**(code **)**(undefined4 **)(local_8 + 0x2c))();
  (**(code **)**(undefined4 **)(local_8 + 0x30))();
  (**(code **)**(undefined4 **)(local_8 + 0x34))();
  (**(code **)(**(int **)(local_8 + 0x34) + 0x38))();
  FUN_00423520((void *)(local_8 + 0x10),0);
  FUN_00423520((void *)(local_8 + 0x44),0);
  *(undefined4 *)(local_8 + 0x40) = 0;
  FUN_00423520((void *)(local_8 + 100),0);
  *(undefined4 *)(local_8 + 0x54) = 0;
  FUN_0049da50((undefined4 *)(local_8 + 0x38));
  FUN_00530580(local_8);
  local_a8[0x10] = (void *)0x1e;
  local_a8[0x11] = (void *)0x1e;
  local_a8[0x12] = (void *)0x28;
  local_a8[0x13] = (void *)0x41;
  local_a8[0x14] = (void *)0x64;
  local_a8[0x15] = (void *)0x3c;
  local_a8[0x16] = (void *)0x46;
  local_a8[0x17] = (void *)0x14;
  local_28 = (void *)FUN_00464080(0);
  iVar1 = FUN_00464080(0);
  iVar1 = FUN_0041cab0(iVar1);
  local_24 = local_a8[iVar1 + 0x10];
  FUN_004be9b0(local_28,local_24);
  local_a8[8] = (void *)0x82;
  local_a8[9] = (void *)0x82;
  local_a8[10] = (void *)0x96;
  local_a8[0xb] = (void *)0x96;
  local_a8[0xc] = (void *)0xb4;
  local_a8[0xd] = (void *)0x82;
  local_a8[0xe] = (void *)0x82;
  local_a8[0xf] = (void *)0x96;
  local_30 = (void *)FUN_00464080(0);
  iVar1 = FUN_00464080(0);
  iVar1 = FUN_0041cab0(iVar1);
  local_2c = local_a8[iVar1 + 8];
  FUN_004bea00(local_30,local_2c);
  local_a8[0] = (void *)0x0;
  local_a8[1] = (void *)0x44c;
  local_a8[2] = (void *)0x4b0;
  local_a8[3] = (void *)0x514;
  local_a8[4] = (void *)0x578;
  local_a8[5] = (void *)0x578;
  local_a8[6] = (void *)0x578;
  local_a8[7] = (void *)0x578;
  local_38 = (void *)FUN_00464080(0);
  local_34 = local_a8[*DAT_005c6110];
  FUN_004be710(local_38,local_34);
  local_40 = (void *)FUN_00464080(0);
  iVar1 = FUN_00464080(0);
  local_3c = FUN_00499520(iVar1);
  local_3c = local_3c / 2;
  FUN_004be620(local_40,local_3c);
  iVar1 = FUN_00464080(0);
  iVar1 = FUN_00499520(iVar1);
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_00513e60(iVar2);
  if (iVar2 + -200 <= iVar1) {
    local_a8[0x18] = (void *)FUN_00464080(0);
    iVar1 = FUN_00464080(0);
    local_44 = FUN_00513e60(iVar1);
    local_44 = local_44 + -200;
    FUN_004be620(local_a8[0x18],local_44);
  }
  return 0;
}

