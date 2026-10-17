/* Entry: 0x0041d210; symbol: FUN_0041d210; body bytes: 214 */

undefined8 __cdecl FUN_0041d210(uint *param_1,uint *param_2,uint *param_3)

{
  bool bVar1;
  char cVar2;
  uint *puVar3;
  ulonglong uVar4;
  uint *local_14;
  uint *local_10;
  uint *local_c;
  
  cVar2 = FUN_00411170();
  if (cVar2 == '\0') {
    uVar4 = FUN_00543e20(param_1,param_2,(int)param_3 << 1);
    puVar3 = (uint *)(uVar4 >> 0x20);
  }
  else {
    bVar1 = true;
    for (local_14 = param_2; puVar3 = (uint *)((int)param_2 + (int)param_3 * 2), local_14 != puVar3;
        local_14 = (uint *)((int)local_14 + 2)) {
      if (param_1 == local_14) {
        bVar1 = false;
        break;
      }
    }
    if (bVar1) {
      local_c = (uint *)0x0;
      while (local_c != param_3) {
        *(undefined2 *)((int)param_1 + (int)local_c * 2) =
             *(undefined2 *)((int)param_2 + (int)local_c * 2);
        puVar3 = (uint *)((int)local_c + 1);
        local_c = puVar3;
      }
    }
    else {
      local_10 = param_3;
      while (local_10 != (uint *)0x0) {
        *(undefined2 *)((int)param_1 + (int)local_10 * 2 + -2) =
             *(undefined2 *)((int)param_2 + (int)local_10 * 2 + -2);
        puVar3 = (uint *)((int)local_10 + -1);
        local_10 = puVar3;
      }
    }
  }
  return CONCAT44(puVar3,param_1);
}

