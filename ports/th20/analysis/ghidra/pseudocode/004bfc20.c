/* Entry: 0x004bfc20; symbol: FUN_004bfc20; body bytes: 66 */

void __thiscall FUN_004bfc20(void *this,int param_1,float *param_2)

{
  undefined4 *puVar1;
  undefined4 local_10;
  undefined4 local_c;
  void *local_8;
  
  local_c = *(undefined4 *)((int)this + 0xb0);
  local_8 = this;
  puVar1 = FUN_00464600(&local_10,(uint *)&DAT_00570c04,param_1,param_2,(undefined4 *)0x0);
  *(undefined4 *)((int)local_8 + param_1 * 4 + 0x70) = *puVar1;
  return;
}

