/* Entry: 0x00411480; symbol: FUN_00411480; body bytes: 199 */

undefined8 __cdecl FUN_00411480(uint *param_1,uint *param_2,undefined1 *param_3)

{
  char cVar1;
  undefined1 *puVar2;
  ulonglong uVar3;
  uint *local_14;
  undefined1 *local_10;
  undefined1 *local_c;
  byte local_5;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    uVar3 = FUN_00543e20(param_1,param_2,(uint)param_3);
    puVar2 = (undefined1 *)(uVar3 >> 0x20);
  }
  else {
    local_5 = 1;
    for (local_14 = param_2; local_14 != (uint *)((int)param_2 + (int)param_3);
        local_14 = (uint *)((int)local_14 + 1)) {
      if (param_1 == local_14) {
        local_5 = 0;
        break;
      }
    }
    puVar2 = (undefined1 *)(uint)local_5;
    if (puVar2 == (undefined1 *)0x0) {
      for (local_10 = param_3; puVar2 = param_3, local_10 != (undefined1 *)0x0;
          local_10 = local_10 + -1) {
        param_3 = (undefined1 *)((int)param_2 + (int)local_10);
        ((undefined1 *)((int)param_1 + (int)local_10))[-1] = param_3[-1];
      }
    }
    else {
      for (local_c = (undefined1 *)0x0; local_c != param_3; local_c = local_c + 1) {
        puVar2 = (undefined1 *)((int)param_1 + (int)local_c);
        *puVar2 = *(undefined1 *)((int)param_2 + (int)local_c);
      }
    }
  }
  return CONCAT44(puVar2,param_1);
}

