/* Entry: 0x00558bf0; symbol: FUN_00558bf0; body bytes: 137 */

void __cdecl
FUN_00558bf0(undefined4 *param_1,char *param_2,uint param_3,char *param_4,int param_5,size_t param_6
            ,int param_7,int *param_8)

{
  int iVar1;
  uint uVar2;
  int local_10;
  int local_c;
  undefined4 uStack_8;
  
  local_10 = 0;
  local_c = 0;
  uStack_8 = 0;
  iVar1 = FUN_0056281d((uint *****)*param_1,param_1[1],param_6,0,&local_10,param_4,param_5);
  uVar2 = param_3 - (local_10 == 0x2d);
  if (param_3 == 0xffffffff) {
    uVar2 = 0xffffffff;
  }
  iVar1 = FUN_00562724((uint *)(param_2 + (local_10 == 0x2d)),uVar2,local_c + param_6,&local_10,
                       iVar1,param_7,param_8);
  if (iVar1 == 0) {
    FUN_00558c79(param_2,param_3,param_6,&local_10,'\0',param_8);
  }
  else {
    *param_2 = '\0';
  }
  return;
}

