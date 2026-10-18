/* Entry: 0x00561c61; symbol: FUN_00561c61; body bytes: 103 */

uint __cdecl FUN_00561c61(int param_1,int param_2,int *param_3)

{
  uint in_EAX;
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  if (-1 < param_2) {
    do {
      in_EAX = (iVar3 + param_2) / 2;
      iVar1 = FUN_0055df10((ushort *)*param_3,*(ushort **)(in_EAX * 0xc + param_1));
      if (iVar1 == 0) {
        iVar3 = param_1 + 4 + in_EAX * 0xc;
        *param_3 = iVar3;
        return CONCAT31((int3)((uint)iVar3 >> 8),1);
      }
      iVar2 = in_EAX - 1;
      if (-1 < iVar1) {
        iVar2 = param_2;
      }
      if (-1 < iVar1) {
        iVar3 = in_EAX + 1;
      }
      param_2 = iVar2;
    } while (iVar3 <= iVar2);
  }
  return in_EAX & 0xffffff00;
}

