/* Entry: 0x00500660; symbol: FUN_00500660; body bytes: 140 */

undefined4 __fastcall FUN_00500660(int param_1)

{
  int iVar1;
  float *pfVar2;
  void *this;
  
  *(undefined4 *)(param_1 + 0xb0) = 0;
  iVar1 = FUN_0040ff90(*(int *)(param_1 + 0x120));
  pfVar2 = (float *)FUN_00460850(iVar1);
  FUN_00426eb0(&DAT_005ba830,0x14,*pfVar2);
  this = (void *)FUN_004c0e10((undefined4 *)(param_1 + 0xdc));
  if (this != (void *)0x0) {
    FUN_0047a4e0(this,4);
    FUN_00470b20(this,0);
  }
  *(uint *)(param_1 + 0x3c) = *(uint *)(param_1 + 0x3c) & 0xfffffffe;
  return 0;
}

