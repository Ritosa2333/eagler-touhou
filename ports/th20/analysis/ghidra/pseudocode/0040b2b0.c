/* Entry: 0x0040b2b0; symbol: FUN_0040b2b0; body bytes: 85 */

void __cdecl FUN_0040b2b0(undefined4 param_1,int param_2,undefined4 param_3)

{
  uint uVar1;
  int iVar2;
  undefined4 local_8;
  
  uVar1 = move<>(param_1);
  while( true ) {
    local_8 = FUN_0040c420(param_1);
    iVar2 = FUN_0040b2a0(&local_8);
    if (param_2 != iVar2) break;
    FUN_005408a2(uVar1,&param_2,4,0xffffffff);
  }
  return;
}

