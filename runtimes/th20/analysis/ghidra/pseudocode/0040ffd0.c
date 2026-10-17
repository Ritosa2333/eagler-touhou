/* Entry: 0x0040ffd0; symbol: FUN_0040ffd0; body bytes: 92 */

undefined8 __cdecl FUN_0040ffd0(uint *param_1,uint *param_2,uint param_3)

{
  char cVar1;
  ulonglong uVar2;
  undefined4 local_8;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    uVar2 = FUN_00543e20(param_1,param_2,param_3);
    local_8 = (uint)(uVar2 >> 0x20);
  }
  else {
    for (local_8 = 0; local_8 != param_3; local_8 = local_8 + 1) {
      *(undefined1 *)((int)param_1 + local_8) = *(undefined1 *)((int)param_2 + local_8);
    }
  }
  return CONCAT44(local_8,param_1);
}

