/* Entry: 0x004d01d0; symbol: FUN_004d01d0; body bytes: 189 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004d01d0(int param_1)

{
  float10 fVar1;
  
  FUN_00470af0((void *)(param_1 + 0x770),(undefined4 *)(param_1 + 0x58));
  fVar1 = (float10)FUN_00438600(*(float *)(param_1 + 0x70),DAT_0056e0f0 / DAT_0056c8d0);
  FUN_00450590((void *)(param_1 + 0x770),(float)fVar1);
  FUN_0044c570((void *)(param_1 + 0x770));
  if (*(float *)(param_1 + 0x80) == _DAT_0056e0e8) {
    FUN_00470af0((void *)(param_1 + 0xd54),(undefined4 *)(param_1 + 0x58));
    FUN_0044c570((void *)(param_1 + 0xd54));
  }
  return 0;
}

