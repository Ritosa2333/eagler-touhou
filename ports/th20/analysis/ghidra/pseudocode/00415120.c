/* Entry: 0x00415120; symbol: FUN_00415120; body bytes: 681 */

void __thiscall FUN_00415120(void *this,int param_1,int param_2,int param_3)

{
  undefined *puVar1;
  int iVar2;
  HBITMAP h;
  HDC hdc;
  HGDIOBJ pvVar3;
  undefined4 *local_98;
  int local_94;
  undefined4 *local_90;
  int local_8c;
  undefined4 *local_78;
  BITMAPINFO local_74;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00416360((int)this);
  _memset(&local_74,0,0x6c);
  puVar1 = FUN_00415640(param_3);
  if (puVar1 != (undefined *)0x0) {
    iVar2 = ((int)(param_1 * *(int *)(puVar1 + 4) + (param_1 * *(int *)(puVar1 + 4) >> 0x1f & 7U))
            >> 3) + 3;
    iVar2 = ((int)(iVar2 + (iVar2 >> 0x1f & 3U)) >> 2) * 4;
    local_74.bmiHeader.biSize = 0x6c;
    local_74.bmiHeader.biWidth = param_1;
    local_74.bmiHeader.biHeight = -(param_2 + 1);
    local_74.bmiHeader.biPlanes = 1;
    local_74.bmiHeader.biBitCount = *(WORD *)(puVar1 + 4);
    local_74.bmiHeader.biSizeImage = param_2 * iVar2;
    if ((param_3 != 0x18) && (param_3 != 0x16)) {
      local_74.bmiHeader.biCompression = 3;
      local_74.bmiColors[0] = *(RGBQUAD *)(puVar1 + 0xc);
      local_48 = *(undefined4 *)(puVar1 + 0x10);
      local_44 = *(undefined4 *)(puVar1 + 0x14);
      local_40 = *(undefined4 *)(puVar1 + 8);
    }
    local_78 = (undefined4 *)0x0;
    h = CreateDIBSection((HDC)0x0,&local_74,0,&local_78,(HANDLE)0x0,0);
    if (h != (HBITMAP)0x0) {
      hdc = CreateCompatibleDC((HDC)0x0);
      pvVar3 = SelectObject(hdc,h);
      *(HDC *)((int)this + 0x114) = hdc;
      *(HBITMAP *)((int)this + 0x11c) = h;
      *(undefined4 **)((int)this + 0x120) = local_78;
      *(DWORD *)((int)this + 0x10c) = local_74.bmiHeader.biSizeImage;
      *(HGDIOBJ *)((int)this + 0x118) = pvVar3;
      *(int *)((int)this + 0x104) = param_1;
      *(int *)((int)this + 0x108) = param_2;
      *(int *)((int)this + 0x100) = param_3;
      *(int *)((int)this + 0x110) = iVar2;
      if (*(int *)((int)this + 0x100) == 0x15) {
        local_90 = local_78;
        for (local_8c = 0; local_8c < *(int *)((int)this + 0x10c); local_8c = local_8c + 4) {
          *local_90 = 0xff000000;
          local_90 = local_90 + 1;
        }
      }
      else if (*(int *)((int)this + 0x100) == 0x1a) {
        local_98 = local_78;
        for (local_94 = 0; local_94 < *(int *)((int)this + 0x10c); local_94 = local_94 + 2) {
          *(undefined2 *)local_98 = 0xf000;
          local_98 = (undefined4 *)((int)local_98 + 2);
        }
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

