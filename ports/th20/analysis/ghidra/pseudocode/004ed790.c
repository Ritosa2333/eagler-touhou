/* Entry: 0x004ed790; symbol: FUN_004ed790; body bytes: 80 */

void __thiscall FUN_004ed790(void *this,char *param_1)

{
  uint uVar1;
  char local_1c [20];
  int *local_8;
  
  local_8 = (int *)this;
  FUN_004fbf70(this,local_1c,param_1);
  uVar1 = FUN_004ec420();
  *(uint *)(*local_8 + 4) = uVar1;
  return;
}

