/* Entry: 0x0047da40; symbol: FUN_0047da40; body bytes: 291 */

void __fastcall FUN_0047da40(void *param_1)

{
  int iVar1;
  void *this;
  
  if (*(int *)((int)param_1 + 0x50) != 0) {
    FUN_00411ce0((int)param_1);
    iVar1 = FUN_0040c300(*(undefined4 **)((int)param_1 + 0x520));
    this = (void *)FUN_00485900(iVar1);
    FUN_0044a6b0(this,param_1);
    *(undefined4 *)((int)param_1 + 0x50) = 0;
    FUN_00423520((void *)((int)param_1 + 0x4f8),0);
    FUN_00423520((void *)((int)param_1 + 0x508),0);
    FUN_00423520((void *)((int)param_1 + 0x4a0),0);
    FUN_00423520((void *)((int)param_1 + 0x4b0),0);
    FUN_00423520((void *)((int)param_1 + 0x4c0),0);
    *(undefined4 *)((int)param_1 + 0x54) = 0;
    *(undefined4 *)((int)param_1 + 0x2c) = 0;
    *(uint *)((int)param_1 + 0x14) = *(uint *)((int)param_1 + 0x14) & 0xfffffffe;
    *(uint *)((int)param_1 + 0x14) = *(uint *)((int)param_1 + 0x14) & 0xfffffeff;
    *(uint *)((int)param_1 + 0x14) = *(uint *)((int)param_1 + 0x14) & 0xfffffdff;
    *(uint *)((int)param_1 + 0x14) = *(uint *)((int)param_1 + 0x14) & 0xffffffbf;
    FUN_0044fcd0((undefined4 *)((int)param_1 + 0x5c));
    FUN_0044c2f0(DAT_005c0028,*(void **)((int)param_1 + 0x1c));
    FUN_004862b0();
  }
  return;
}

