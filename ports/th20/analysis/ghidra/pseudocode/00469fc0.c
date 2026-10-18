/* Entry: 0x00469fc0; symbol: FUN_00469fc0; body bytes: 104 */

uint FUN_00469fc0(void)

{
  bool bVar1;
  int *piVar2;
  undefined3 extraout_var;
  int *local_8;
  int *piVar3;
  
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000004,(char **)&stack0x00000008);
  local_8 = (int *)FUN_004690e0((undefined4 *)&stack0x00000004);
  piVar2 = (int *)FUN_004690e0((undefined4 *)&stack0x00000008);
  piVar3 = piVar2;
  while( true ) {
    if (local_8 == piVar2) {
      return (uint)piVar3 & 0xffffff00;
    }
    bVar1 = FUN_0046b330(&stack0x0000000c,*local_8);
    piVar3 = (int *)CONCAT31(extraout_var,bVar1);
    if (bVar1) break;
    local_8 = local_8 + 1;
  }
  return CONCAT31(extraout_var,1);
}

