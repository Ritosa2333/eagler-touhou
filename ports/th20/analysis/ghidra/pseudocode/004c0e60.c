/* Entry: 0x004c0e60; symbol: FUN_004c0e60; body bytes: 225 */

void * __fastcall FUN_004c0e60(int param_1)

{
  undefined4 *puVar1;
  void *this;
  
  puVar1 = (undefined4 *)FUN_004859f0(param_1 + 0xc42c);
  if (puVar1 == (undefined4 *)0x0) {
    this = FUN_004bfe30();
    FUN_0044a670(this,this);
    FUN_00411ea0((void *)(param_1 + 0xc414),this);
    FUN_004117a0((void *)((int)this + 0x8c),*(uint *)(param_1 + 0xc410) | 0x1000000);
    FUN_00488a20(this,0);
    FUN_004c0c60(param_1);
  }
  else {
    FUN_00411ce0((int)puVar1);
    FUN_00411ea0((void *)(param_1 + 0xc414),puVar1);
    this = (void *)FUN_0040c300(puVar1);
    FUN_004117a0((void *)((int)this + 0x8c),*(undefined4 *)(param_1 + 0xc410));
    FUN_004c0c60(param_1);
  }
  return this;
}

