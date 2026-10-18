/* Entry: 0x00535c50; symbol: FUN_00535c50; body bytes: 75 */

undefined4 __thiscall FUN_00535c50(void *this,undefined4 *param_1)

{
  int iVar1;
  void *this_00;
  
  iVar1 = FUN_00460830(0);
  this_00 = (void *)FUN_004ff6b0(iVar1);
                    /* WARNING: Load size is inaccurate */
  FUN_00505e40(this_00,*(int *)(*this + 4) * 0x14 + 0xf,0,0,param_1);
  return 1;
}

