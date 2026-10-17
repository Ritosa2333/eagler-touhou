/* Entry: 0x00539ab0; symbol: FUN_00539ab0; body bytes: 108 */

void FUN_00539ab0(NumaInformation *param_1)

{
  undefined4 uVar1;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (param_1 != (NumaInformation *)0x0) {
    uVar1 = FUN_0040c310(&DAT_005c0240,1);
    FUN_0040b730(&local_c,uVar1);
    if (param_1 != (NumaInformation *)0x0) {
      Concurrency::details::SchedulerBase::NumaInformation::_vector_deleting_destructor_(param_1,3);
    }
    FUN_0040b900(&local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

