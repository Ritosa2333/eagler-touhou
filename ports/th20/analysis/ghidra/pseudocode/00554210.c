/* Entry: 0x00554210; symbol: FUN_00554210; body bytes: 255 */

float10 __cdecl
FUN_00554210(int param_1,int param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
            undefined4 param_6,undefined4 param_7,undefined4 param_8)

{
  int iVar1;
  __acrt_ptd *p_Var2;
  int local_24;
  int local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 uStack_8;
  
  iVar1 = 0;
  do {
    if ((&DAT_00599490)[iVar1 * 2] == param_2) {
      local_20 = *(int *)(&UNK_00599494 + iVar1 * 8);
      if (local_20 != 0) {
        local_1c = param_3;
        local_18 = param_4;
        local_14 = param_5;
        local_10 = param_6;
        local_c = param_7;
        local_24 = param_1;
        uStack_8 = param_8;
        FUN_0055dbe0();
        iVar1 = FUN_00555940(&local_24);
        if (iVar1 == 0) {
          if (param_1 == 1) {
            p_Var2 = FUN_005516c1();
            *(undefined4 *)p_Var2 = 0x21;
          }
          else if ((param_1 == 2) || (param_1 == 3)) {
            p_Var2 = FUN_005516c1();
            *(undefined4 *)p_Var2 = 0x22;
            return (float10)(double)CONCAT44(uStack_8,local_c);
          }
        }
        return (float10)(double)CONCAT44(uStack_8,local_c);
      }
      goto LAB_00554236;
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x1d);
  local_20 = 0;
LAB_00554236:
  FUN_0055dbe0();
  if (param_1 == 1) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x21;
  }
  else if ((param_1 == 2) || (param_1 == 3)) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x22;
    return (float10)(double)CONCAT44(param_8,param_7);
  }
  return (float10)(double)CONCAT44(param_8,param_7);
}

