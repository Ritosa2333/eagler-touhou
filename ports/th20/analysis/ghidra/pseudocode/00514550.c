/* Entry: 0x00514550; symbol: FUN_00514550; body bytes: 147 */

int __cdecl FUN_00514550(int param_1,int param_2,void *param_3)

{
  char cVar1;
  undefined4 *puVar2;
  undefined1 local_30 [8];
  undefined4 local_28;
  undefined4 local_24;
  uint *local_20;
  uint local_1c;
  undefined1 local_12;
  undefined1 local_11;
  
  local_11 = 0;
  local_12 = 1;
  while( true ) {
    if (param_1 == param_2) {
      return param_1;
    }
    puVar2 = (undefined4 *)FUN_004a3e60(param_3,local_30);
    local_20 = (uint *)*puVar2;
    local_1c = puVar2[1];
    puVar2 = (undefined4 *)_Dest_val<>(&stack0x00000010,param_1);
    local_28 = *puVar2;
    local_24 = puVar2[1];
    cVar1 = FUN_004a1580(local_28,local_24,local_20,local_1c);
    if (cVar1 != '\0') break;
    param_1 = param_1 + 8;
  }
  return param_1;
}

