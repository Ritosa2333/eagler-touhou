/* Entry: 0x004d99d0; symbol: FUN_004d99d0; body bytes: 151 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_004d99d0(float param_1)

{
  float10 fVar1;
  float local_8;
  
  fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
  if ((float)fVar1 == _DAT_0056e0e8) {
    local_8 = param_1;
  }
  else {
    fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
    if ((float)fVar1 <= DAT_0056c8cc) {
      fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
      local_8 = param_1 / (float)fVar1;
    }
    else {
      local_8 = param_1;
    }
  }
  FUN_00428c90(&DAT_005ba830,(char **)0x5,(int)local_8,0x570afc);
  return 0;
}

