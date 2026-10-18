/* Entry: 0x00539a10; symbol: FUN_00539a10; body bytes: 153 */

void __cdecl FUN_00539a10(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *(int *)(&DAT_005c8b38 + param_1 * 0xc);
  if (*(int *)(&DAT_005c8b3c + iVar1 * 0xc) == param_1) {
    *(int *)(&DAT_005c8b3c + iVar1 * 0xc) = param_2;
  }
  else {
    *(int *)(&DAT_005c8b40 + iVar1 * 0xc) = param_2;
  }
  iVar2 = param_1 * 0xc;
  iVar1 = param_2 * 0xc;
  *(undefined4 *)(&DAT_005c8b38 + iVar1) = *(undefined4 *)(&DAT_005c8b38 + iVar2);
  *(undefined4 *)(&DAT_005c8b3c + iVar1) = *(undefined4 *)(&DAT_005c8b3c + iVar2);
  *(undefined4 *)(&DAT_005c8b40 + iVar1) = *(undefined4 *)(&DAT_005c8b40 + iVar2);
  *(int *)(&DAT_005c8b38 + *(int *)(&DAT_005c8b3c + param_2 * 0xc) * 0xc) = param_2;
  *(int *)(&DAT_005c8b38 + *(int *)(&DAT_005c8b40 + param_2 * 0xc) * 0xc) = param_2;
  *(undefined4 *)(&DAT_005c8b38 + param_1 * 0xc) = 0;
  return;
}

