/* Entry: 0x00557210; symbol: FUN_00557210; body bytes: 108 */

undefined4 __cdecl FUN_00557210(uint *param_1,uint param_2,uint param_3)

{
  __acrt_ptd *p_Var1;
  uint uVar2;
  
  uVar2 = param_3 & 0xfff7ffff;
  if ((param_2 & uVar2 & 0xfcf0fce0) != 0) {
    if (param_1 != (uint *)0x0) {
      uVar2 = FUN_005620b0(0,0);
      *param_1 = uVar2;
    }
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return 0x16;
  }
  if (param_1 != (uint *)0x0) {
    uVar2 = FUN_005620b0(param_2,uVar2);
    *param_1 = uVar2;
    return 0;
  }
  FUN_005620b0(param_2,uVar2);
  return 0;
}

