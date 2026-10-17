/* Entry: 0x00478300; symbol: FUN_00478300; body bytes: 121 */

undefined4 * __cdecl FUN_00478300(int param_1)

{
  undefined4 *this;
  int iVar1;
  void *this_00;
  
  this = FUN_00477960();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_00477dc0(this,param_1);
    if (iVar1 == 0) {
      this_00 = (void *)FUN_0040bbc0(&DAT_005ba568,param_1);
      FUN_0041de90(this_00,this);
    }
    else {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

