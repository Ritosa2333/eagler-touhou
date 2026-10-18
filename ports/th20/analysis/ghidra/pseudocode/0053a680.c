/* Entry: 0x0053a680; symbol: FUN_0053a680; body bytes: 103 */

int __fastcall FUN_0053a680(int *param_1)

{
  int iVar1;
  size_t sVar2;
  uint uVar3;
  undefined4 local_8;
  
  sVar2 = _strlen((char *)*param_1);
  local_8 = sVar2 + 1;
  uVar3 = local_8 & 0x80000003;
  if ((int)uVar3 < 0) {
    uVar3 = (uVar3 - 1 | 0xfffffffc) + 1;
  }
  if (uVar3 != 0) {
    uVar3 = local_8 & 0x80000003;
    if ((int)uVar3 < 0) {
      uVar3 = (uVar3 - 1 | 0xfffffffc) + 1;
    }
    local_8 = (4 - uVar3) + local_8;
  }
  iVar1 = *param_1;
  *param_1 = iVar1 + local_8;
  return iVar1 + local_8;
}

