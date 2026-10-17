/* Entry: 0x0053a6f0; symbol: FUN_0053a6f0; body bytes: 305 */

void __thiscall FUN_0053a6f0(void *this,char **param_1)

{
  char cVar1;
  undefined4 uVar2;
  char *pcVar3;
  char *local_408 [256];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0053a170((undefined4 *)this);
  GetCurrentDirectoryW(0x200,(LPWSTR)local_408);
  std::_Adl_verify_range<char*,char_const*>((char **)"info : CurrentDirectory %s \r\n",local_408);
  std::_Adl_verify_range<char*,char_const*>((char **)"info : %s open arcfile\r\n",param_1);
  uVar2 = FUN_00539c80();
  *(undefined4 *)((int)this + 0xc) = uVar2;
  if (*(int *)((int)this + 0xc) != 0) {
    cVar1 = FUN_00539ed0(this,param_1);
    if (cVar1 != '\0') {
      pcVar3 = FUN_00539e60((char *)param_1);
      *(char **)((int)this + 8) = pcVar3;
      if (*(int *)((int)this + 8) != 0) {
        (**(code **)**(undefined4 **)((int)this + 0xc))
                  (*(undefined4 *)((int)this + 8),PTR_DAT_005b0af8,
                   *(code **)**(undefined4 **)((int)this + 0xc),*(undefined4 *)((int)this + 0xc));
        goto LAB_0053a811;
      }
    }
    std::_Adl_verify_range<char*,char_const*>((char **)"info : %s not found\r\n",param_1);
    FUN_0053a170((undefined4 *)this);
  }
LAB_0053a811:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

