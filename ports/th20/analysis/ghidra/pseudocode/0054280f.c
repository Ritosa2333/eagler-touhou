/* Entry: 0x0054280f; symbol: ___scrt_initialize_crt; body bytes: 57 */

/* Library Function - Single Match
    ___scrt_initialize_crt
   
   Library: Visual Studio 2019 Release */

uint __cdecl ___scrt_initialize_crt(int param_1)

{
  uint uVar1;
  undefined4 uVar2;
  
  if (param_1 == 0) {
    DAT_005e4f4c = 1;
  }
  FUN_005436c1();
  uVar1 = ___vcrt_initialize();
  if ((char)uVar1 != '\0') {
    uVar2 = ___acrt_initialize();
    if ((char)uVar2 != '\0') {
      return CONCAT31((int3)((uint)uVar2 >> 8),1);
    }
    uVar1 = ___vcrt_uninitialize('\0');
  }
  return uVar1 & 0xffffff00;
}

