/* Entry: 0x004e92a0; symbol: FUN_004e92a0; body bytes: 597 */

undefined8 __cdecl FUN_004e92a0(uint *param_1,uint *param_2)

{
  byte bVar1;
  uint *puVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  longlong lVar5;
  undefined8 uVar6;
  uint *local_78;
  undefined4 local_74;
  uint local_30;
  uint local_2c;
  uint local_28;
  uint local_24;
  uint local_c;
  undefined1 local_5;
  
  uVar3 = FUN_004e7ae0((undefined8 *)&stack0x0000000c);
  local_30 = (uint)uVar3;
  local_2c = (uint)(uVar3 >> 0x20);
  if (local_30 == 0 && local_2c == 0) {
    if ((int)param_2 - (int)param_1 < 4) {
      local_74 = 0x84;
    }
    else {
      FUN_00543e20(param_1,(uint *)&DAT_00572954,4);
      param_2 = param_1 + 1;
      local_74 = 0;
    }
  }
  else {
    local_24 = local_2c & 0xfffff;
    uVar4 = __aullshr(0x34,local_2c);
    if ((int)uVar4 == 0) {
      local_5 = 0x30;
      local_c = 0xfffffc02;
    }
    else {
      local_5 = 0x31;
      local_c = (int)uVar4 - 0x3ff;
    }
    if (param_1 == param_2) {
      local_74 = 0x84;
    }
    else {
      *(undefined1 *)param_1 = local_5;
      puVar2 = (uint *)((int)param_1 + 1);
      if (local_30 != 0 || (uVar3 & 0xfffff00000000) != 0) {
        if (puVar2 == param_2) {
          local_74 = 0x84;
          goto LAB_004e94f1;
        }
        *(undefined1 *)puVar2 = 0x2e;
        param_1 = (uint *)((int)param_1 + 2);
        local_28 = local_30;
        bVar1 = 0x34;
        do {
          bVar1 = bVar1 - 4;
          uVar3 = __aullshr(bVar1,local_24);
          if (param_1 == param_2) {
            local_74 = 0x84;
            goto LAB_004e94f1;
          }
          *(char *)param_1 =
               "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
               [(int)uVar3];
          param_1 = (uint *)((int)param_1 + 1);
          lVar5 = __allshl(bVar1,0);
          local_28 = local_28 & (int)lVar5 - 1U;
          local_24 = local_24 & (int)((ulonglong)lVar5 >> 0x20) - (uint)((int)lVar5 == 0);
          puVar2 = param_1;
        } while (local_28 != 0 || local_24 != 0);
      }
      param_1 = puVar2;
      if ((int)param_2 - (int)param_1 < 2) {
        local_74 = 0x84;
      }
      else {
        *(undefined1 *)param_1 = 0x70;
        if ((int)local_c < 0) {
          *(undefined1 *)((int)param_1 + 1) = 0x2d;
          local_c = -local_c;
        }
        else {
          *(undefined1 *)((int)param_1 + 1) = 0x2b;
        }
        param_1 = (uint *)((int)param_1 + 2);
        uVar6 = FUN_004710c0((char *)param_1,(char *)param_2,local_c,10);
        local_78 = (uint *)uVar6;
        local_74 = (undefined4)((ulonglong)uVar6 >> 0x20);
        param_2 = local_78;
      }
    }
  }
LAB_004e94f1:
  return CONCAT44(local_74,param_2);
}

