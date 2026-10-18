/* Entry: 0x005583d2; symbol: FUN_005583d2; body bytes: 35 */

bool FUN_005583d2(void)

{
  BOOL BVar1;
  DWORD local_8;
  
  BVar1 = VirtualProtect(&DAT_005e6000,0x80,2,&local_8);
  return BVar1 != 0;
}

