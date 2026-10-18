/* Entry: 0x0044c8f0; symbol: FUN_0044c8f0; body bytes: 187 */

undefined4 * __thiscall FUN_0044c8f0(void *this,undefined4 *param_1,int param_2)

{
  bool bVar1;
  uint uVar2;
  
  FUN_00449570((int *)((int)this + 0x7d40e88));
  Concurrency::details::HardwareAffinity::IntersectWith
            ((HardwareAffinity *)((int)this + 0x7d40e88),0xffff);
  bVar1 = FUN_00449430((void *)((int)this + 0x7d40e88),0);
  if (bVar1) {
    FUN_00449570((int *)((int)this + 0x7d40e88));
  }
  uVar2 = FUN_004493e0((void *)((int)this + 0x7d40e88),0x10);
  FUN_004117a0((void *)(param_2 + 0x4c0),uVar2 | *(uint *)(param_2 + 0x4c4) & 0xffff);
  *param_1 = *(undefined4 *)(param_2 + 0x4c0);
  return param_1;
}

