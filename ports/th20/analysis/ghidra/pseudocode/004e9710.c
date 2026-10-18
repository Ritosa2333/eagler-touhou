/* Entry: 0x004e9710; symbol: FUN_004e9710; body bytes: 253 */

undefined4 * __cdecl
FUN_004e9710(undefined4 *param_1,undefined4 param_2,byte param_3,int param_4,undefined4 param_5,
            uint param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  undefined1 local_30 [8];
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  byte local_11;
  
  uVar5 = param_8;
  uVar4 = param_7;
  uVar3 = param_6;
  uVar2 = param_5;
  iVar1 = param_4;
  if (((char)param_6 == '\0') || ((char)param_6 == 'c')) {
    puVar6 = FUN_0040de00(local_30,&param_3,1);
    local_28 = *puVar6;
    local_24 = puVar6[1];
    puVar6 = (undefined4 *)move<>(&param_2);
    local_20 = *puVar6;
    FUN_004e9b30(param_1,local_20,local_28,local_24,&param_4);
  }
  else {
    local_11 = param_3;
    puVar6 = (undefined4 *)move<>(&param_2);
    local_1c = *puVar6;
    local_18 = local_1c;
    FUN_004f0020(param_1,local_1c,local_11,iVar1,uVar2,uVar3,uVar4,uVar5,param_9);
  }
  return param_1;
}

