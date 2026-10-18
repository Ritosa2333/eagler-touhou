/* Entry: 0x004e9080; symbol: FUN_004e9080; body bytes: 534 */

undefined8 __cdecl FUN_004e9080(uint *param_1,uint *param_2)

{
  uint uVar1;
  byte bVar2;
  uint *puVar3;
  undefined8 uVar4;
  uint *local_68;
  undefined4 local_64;
  uint local_14;
  uint local_c;
  undefined1 local_5;
  
  uVar1 = FUN_0040c990((undefined4 *)&stack0x0000000c);
  if (uVar1 == 0) {
    if ((int)param_2 - (int)param_1 < 4) {
      local_64 = 0x84;
    }
    else {
      FUN_00543e20(param_1,(uint *)&DAT_00572954,4);
      param_2 = param_1 + 1;
      local_64 = 0;
    }
  }
  else {
    if (uVar1 >> 0x17 == 0) {
      local_5 = 0x30;
      local_c = 0xffffff82;
    }
    else {
      local_5 = 0x31;
      local_c = (uVar1 >> 0x17) - 0x7f;
    }
    if (param_1 == param_2) {
      local_64 = 0x84;
    }
    else {
      *(undefined1 *)param_1 = local_5;
      puVar3 = (uint *)((int)param_1 + 1);
      if ((uVar1 & 0x7fffff) != 0) {
        if (puVar3 == param_2) {
          local_64 = 0x84;
          goto LAB_004e9292;
        }
        *(undefined1 *)puVar3 = 0x2e;
        param_1 = (uint *)((int)param_1 + 2);
        local_14 = (uVar1 & 0x7fffff) << 1;
        bVar2 = 0x18;
        do {
          bVar2 = bVar2 - 4;
          if (param_1 == param_2) {
            local_64 = 0x84;
            goto LAB_004e9292;
          }
          *(char *)param_1 =
               "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
               [local_14 >> (bVar2 & 0x1f)];
          param_1 = (uint *)((int)param_1 + 1);
          local_14 = local_14 & (1 << (bVar2 & 0x1f)) - 1U;
          puVar3 = param_1;
        } while (local_14 != 0);
      }
      param_1 = puVar3;
      if ((int)param_2 - (int)param_1 < 2) {
        local_64 = 0x84;
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
        uVar4 = FUN_004710c0((char *)param_1,(char *)param_2,local_c,10);
        local_68 = (uint *)uVar4;
        local_64 = (undefined4)((ulonglong)uVar4 >> 0x20);
        param_2 = local_68;
      }
    }
  }
LAB_004e9292:
  return CONCAT44(local_64,param_2);
}

