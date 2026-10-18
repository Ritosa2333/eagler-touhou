/* Entry: 0x0046d020; symbol: FUN_0046d020; body bytes: 103 */

void __thiscall FUN_0046d020(void *this,undefined4 *param_1,uint param_2,int param_3,int param_4)

{
  char *pcVar1;
  longlong lVar2;
  
  pcVar1 = (char *)FUN_00470620((int)this);
  lVar2 = __allmul(param_2,param_3,10,0);
  FUN_00453500(pcVar1,0x100,lVar2 + param_4);
  FUN_0040ddd0(&stack0xffffffe8,pcVar1);
  FUN_0046ce90(this,param_1);
  return;
}

