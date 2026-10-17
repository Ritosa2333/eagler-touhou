/* Entry: 0x00477d30; symbol: FUN_00477d30; body bytes: 139 */

undefined4 __thiscall FUN_00477d30(void *this,undefined4 param_1)

{
  int iVar1;
  void *this_00;
  undefined4 *puVar2;
  
  FUN_00478290(this,param_1);
  FUN_00423520((void *)((int)this + 8),0);
  FUN_004117a0((void *)((int)this + 0xac),0);
  iVar1 = FUN_00411700(*(int *)((int)this + 0xb4));
  FUN_00487bb0(iVar1);
  this_00 = (void *)FUN_00412730(*(int *)((int)this + 0xb4));
  FUN_00477f40(this_00,1);
  iVar1 = FUN_0040ff90(*(int *)((int)this + 0xb4));
  puVar2 = (undefined4 *)FUN_00460850(iVar1);
  FUN_00477b80((void *)((int)this + 0x28),puVar2);
  return 0;
}

