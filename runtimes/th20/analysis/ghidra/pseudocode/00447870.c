/* Entry: 0x00447870; symbol: FUN_00447870; body bytes: 62 */

void * __cdecl FUN_00447870(void *param_1,undefined8 *param_2)

{
  undefined8 uVar1;
  undefined8 local_10;
  undefined1 local_6;
  undefined1 local_5;
  
  local_5 = 1;
  local_6 = 0;
  uVar1 = FUN_0044c090(param_2);
  local_10 = __alldiv((uint)uVar1,(uint)((ulonglong)uVar1 >> 0x20),1000,0);
  FUN_00446da0(param_1,(undefined4 *)&local_10);
  return param_1;
}

