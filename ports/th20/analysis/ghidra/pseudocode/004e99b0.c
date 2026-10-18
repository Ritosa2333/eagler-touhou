/* Entry: 0x004e99b0; symbol: FUN_004e99b0; body bytes: 295 */

void __cdecl FUN_004e99b0(undefined4 *param_1,undefined4 param_2,uint param_3,int *param_4)

{
  char *pcVar1;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  char local_3d;
  undefined8 local_3c;
  undefined1 local_34 [24];
  char *local_1c;
  undefined1 local_18;
  undefined1 local_17;
  char local_11;
  char local_10 [8];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar4 = 0x10;
  uVar3 = param_3;
  pcVar1 = (char *)FUN_004a2a90((int)local_10);
  local_3c = FUN_004710c0(local_10,pcVar1,uVar3,uVar4);
  local_1c = (char *)local_3c + (2 - (int)local_10);
  local_18 = 0x30;
  local_17 = 0x78;
  if ((char)param_4[2] == 'P') {
    local_17 = 0x58;
    FUN_004fb8c0(local_10,(char *)local_3c);
  }
  if ((*(char *)((int)param_4 + 0xd) == '\0') || (*(char *)((int)param_4 + 9) != '\0')) {
    local_3d = '\0';
  }
  else {
    local_3d = '\x01';
  }
  local_11 = local_3d;
  FUN_004f3be0(local_34,&local_18,&local_11,&local_1c,param_4,local_10,&local_3c);
  pcVar1 = local_1c;
  if (local_11 == '\0') {
    puVar2 = (undefined4 *)move<>(&param_2);
    FUN_004ef520(param_1,*puVar2,(int)pcVar1,param_4,'\x02');
  }
  else {
    puVar2 = (undefined4 *)move<>(&param_2);
    FUN_004f5a00(param_1,*puVar2);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

