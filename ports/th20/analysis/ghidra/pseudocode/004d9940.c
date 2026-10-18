/* Entry: 0x004d9940; symbol: FUN_004d9940; body bytes: 139 */

void __thiscall FUN_004d9940(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  uint uVar2;
  int in_stack_ffffffec;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_0040c310(&DAT_005c0240,10);
  FUN_0040b730(&local_c,uVar1);
  *(undefined4 *)((int)this + 8) = 0;
  uVar2 = eof(in_stack_ffffffec);
  *(uint *)((int)this + 0xc) = uVar2 >> 1;
  *(int *)((int)this + 0x10) = *(int *)((int)this + 0xc) - *(int *)((int)this + 8);
  *(undefined4 *)((int)this + 0x14) = param_1;
  FUN_004dda10((void *)((int)this + 4),*(uint *)((int)this + 0x14));
  FUN_0040b900(&local_c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

