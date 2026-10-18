/* Entry: 0x00416360; symbol: FUN_00416360; body bytes: 182 */

bool __fastcall FUN_00416360(int param_1)

{
  bool bVar1;
  
  bVar1 = *(int *)(param_1 + 0x114) != 0;
  if (bVar1) {
    SelectObject(*(HDC *)(param_1 + 0x114),*(HGDIOBJ *)(param_1 + 0x118));
    DeleteDC(*(HDC *)(param_1 + 0x114));
    DeleteObject(*(HGDIOBJ *)(param_1 + 0x11c));
    *(undefined4 *)(param_1 + 0x100) = 0xffffffff;
    *(undefined4 *)(param_1 + 0x104) = 0;
    *(undefined4 *)(param_1 + 0x108) = 0;
    *(undefined4 *)(param_1 + 0x114) = 0;
    *(undefined4 *)(param_1 + 0x11c) = 0;
    *(undefined4 *)(param_1 + 0x118) = 0;
    *(undefined4 *)(param_1 + 0x120) = 0;
  }
  return bVar1;
}

