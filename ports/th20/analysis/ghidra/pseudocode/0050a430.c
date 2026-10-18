/* Entry: 0x0050a430; symbol: FUN_0050a430; body bytes: 107 */

void __thiscall FUN_0050a430(void *this,undefined4 *param_1)

{
  undefined4 uVar1;
  
  uVar1 = param_1[1];
  *(undefined4 *)((int)this + 0x620) = *param_1;
  *(undefined4 *)((int)this + 0x624) = uVar1;
  *(float *)((int)this + 0x614) = (float)*(int *)((int)this + 0x620) / DAT_0056cd98;
  *(float *)((int)this + 0x618) = (float)*(int *)((int)this + 0x624) / DAT_0056cd98;
  FUN_004fb290(this,1);
  return;
}

