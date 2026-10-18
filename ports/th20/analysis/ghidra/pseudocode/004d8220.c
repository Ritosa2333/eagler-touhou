/* Entry: 0x004d8220; symbol: FUN_004d8220; body bytes: 152 */

undefined4 __fastcall FUN_004d8220(int param_1)

{
  int *piVar1;
  void *this;
  void *this_00;
  int iVar2;
  int local_c;
  int local_8;
  
  local_8 = param_1;
  piVar1 = FUN_00412310(7,FUN_004d85a0,param_1);
  *(int **)(local_8 + 8) = piVar1;
  piVar1 = FUN_004123b0(0x58,FUN_004d85b0,local_8);
  *(int **)(local_8 + 0xc) = piVar1;
  FUN_0050fce0();
  local_c = local_8;
  FUN_0040b1d0(FUN_004d8350,&local_c);
  this = (void *)FUN_0040bbc0(&DAT_005ba568,0);
  this_00 = (void *)FUN_00498f40();
  iVar2 = FUN_00488720(this_00,0);
  FUN_0041df50(this,iVar2);
  return 0;
}

