/* Entry: 0x0046b590; symbol: FUN_0046b590; body bytes: 332 */

void __cdecl FUN_0046b590(float *param_1,int param_2,void *param_3,uint param_4,int param_5)

{
  char cVar1;
  bool bVar2;
  undefined3 extraout_var;
  uint uVar3;
  
  cVar1 = FUN_0046a4d0();
  if (cVar1 == '\0') {
    bVar2 = FUN_0046b1b0(param_3,param_2);
    if (CONCAT31(extraout_var,bVar2) == 0) {
      FUN_00470ad0(DAT_005c0698,0xff608080);
      FUN_00470ab0(DAT_005c0698,0xff404040);
    }
    else {
      if (param_4 == 0) {
        FUN_00470ad0(DAT_005c0698,0xff80ffff);
        FUN_00470ab0(DAT_005c0698,0xff000000);
      }
      else {
        uVar3 = param_4 & 0x80000003;
        if ((int)uVar3 < 0) {
          uVar3 = (uVar3 - 1 | 0xfffffffc) + 1;
        }
        if ((int)uVar3 < 2) {
          FUN_00470ad0(DAT_005c0698,0xff000000);
          FUN_00470ab0(DAT_005c0698,0xffffff80);
        }
        else {
          FUN_00470ad0(DAT_005c0698,0xff80ffff);
          FUN_00470ab0(DAT_005c0698,0xffffffff);
        }
      }
      if (0 < param_5) {
        *param_1 = *(float *)(&DAT_0056f680 + param_5 * 8) / DAT_0056c8d0 + *param_1;
        param_1[1] = *(float *)(&DAT_0056f680 + param_5 * 8) / DAT_0056c8d0 + param_1[1];
      }
    }
  }
  else {
    FUN_00470ad0(DAT_005c0698,0xff808080);
    FUN_00470ab0(DAT_005c0698,0x40ffffff);
  }
  return;
}

