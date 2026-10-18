/* Entry: 0x005527e3; symbol: FUN_005527e3; body bytes: 33 */

undefined4 __cdecl FUN_005527e3(undefined4 *param_1,SIZE_T param_2)

{
  LPVOID pvVar1;
  undefined4 uVar2;
  
  pvVar1 = FUN_005584c0(param_2);
  *param_1 = pvVar1;
  uVar2 = 0;
  if (pvVar1 == (LPVOID)0x0) {
    uVar2 = 0xc;
  }
  return uVar2;
}

