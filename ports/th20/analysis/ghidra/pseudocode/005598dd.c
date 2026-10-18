/* Entry: 0x005598dd; symbol: FUN_005598dd; body bytes: 176 */

void FUN_005598dd(void)

{
  HANDLE hFile;
  int iVar1;
  uint uVar2;
  DWORD DVar3;
  
  uVar2 = 0;
  do {
    iVar1 = (uVar2 & 0x3f) * 0x38 + (&DAT_005e5690)[(int)uVar2 >> 6];
    if ((*(int *)(iVar1 + 0x18) == -1) || (*(int *)(iVar1 + 0x18) == -2)) {
      *(undefined1 *)(iVar1 + 0x28) = 0x81;
      if (uVar2 == 0) {
        DVar3 = 0xfffffff6;
      }
      else if (uVar2 == 1) {
        DVar3 = 0xfffffff5;
      }
      else {
        DVar3 = 0xfffffff4;
      }
      hFile = GetStdHandle(DVar3);
      if ((hFile != (HANDLE)0xffffffff) && (hFile != (HANDLE)0x0)) {
        DVar3 = GetFileType(hFile);
        if (DVar3 != 0) {
          *(HANDLE *)(iVar1 + 0x18) = hFile;
          if ((DVar3 & 0xff) == 2) {
            *(byte *)(iVar1 + 0x28) = *(byte *)(iVar1 + 0x28) | 0x40;
          }
          else if ((DVar3 & 0xff) == 3) {
            *(byte *)(iVar1 + 0x28) = *(byte *)(iVar1 + 0x28) | 8;
          }
          goto LAB_0055997f;
        }
      }
      *(byte *)(iVar1 + 0x28) = *(byte *)(iVar1 + 0x28) | 0x40;
      *(undefined4 *)(iVar1 + 0x18) = 0xfffffffe;
      if (DAT_005e531c != 0) {
        *(undefined4 *)(*(int *)(DAT_005e531c + uVar2 * 4) + 0x10) = 0xfffffffe;
      }
    }
    else {
      *(byte *)(iVar1 + 0x28) = *(byte *)(iVar1 + 0x28) | 0x80;
    }
LAB_0055997f:
    uVar2 = uVar2 + 1;
    if (uVar2 == 3) {
      return;
    }
  } while( true );
}

