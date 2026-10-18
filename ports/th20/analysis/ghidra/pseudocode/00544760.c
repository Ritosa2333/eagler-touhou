/* Entry: 0x00544760; symbol: _memset; body bytes: 346 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Library Function - Single Match
    _memset
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void * __cdecl _memset(void *_Dst,int _Val,size_t _Size)

{
  int iVar1;
  undefined1 *puVar2;
  int *piVar3;
  
  if (_Size == 0) {
    return _Dst;
  }
  iVar1 = (_Val & 0xffU) * 0x1010101;
  piVar3 = (int *)_Dst;
  if (0x20 < _Size) {
    if (0x7f < _Size) {
      puVar2 = (undefined1 *)_Dst;
      if ((_DAT_005e528c >> 1 & 1) != 0) {
        for (; _Size != 0; _Size = _Size - 1) {
          *puVar2 = (char)iVar1;
          puVar2 = puVar2 + 1;
        }
        return _Dst;
      }
      if ((DAT_005b2618 >> 1 & 1) == 0) goto joined_r0x0054486b;
      *(int *)_Dst = iVar1;
      *(int *)((int)_Dst + 4) = iVar1;
      *(int *)((int)_Dst + 8) = iVar1;
      *(int *)((int)_Dst + 0xc) = iVar1;
      piVar3 = (int *)((int)_Dst + 0x10U & 0xfffffff0);
      _Size = (int)_Dst + (_Size - (int)piVar3);
      if (0x80 < _Size) {
        do {
          *piVar3 = iVar1;
          piVar3[1] = iVar1;
          piVar3[2] = iVar1;
          piVar3[3] = iVar1;
          piVar3[4] = iVar1;
          piVar3[5] = iVar1;
          piVar3[6] = iVar1;
          piVar3[7] = iVar1;
          piVar3[8] = iVar1;
          piVar3[9] = iVar1;
          piVar3[10] = iVar1;
          piVar3[0xb] = iVar1;
          piVar3[0xc] = iVar1;
          piVar3[0xd] = iVar1;
          piVar3[0xe] = iVar1;
          piVar3[0xf] = iVar1;
          piVar3[0x10] = iVar1;
          piVar3[0x11] = iVar1;
          piVar3[0x12] = iVar1;
          piVar3[0x13] = iVar1;
          piVar3[0x14] = iVar1;
          piVar3[0x15] = iVar1;
          piVar3[0x16] = iVar1;
          piVar3[0x17] = iVar1;
          piVar3[0x18] = iVar1;
          piVar3[0x19] = iVar1;
          piVar3[0x1a] = iVar1;
          piVar3[0x1b] = iVar1;
          piVar3[0x1c] = iVar1;
          piVar3[0x1d] = iVar1;
          piVar3[0x1e] = iVar1;
          piVar3[0x1f] = iVar1;
          piVar3 = piVar3 + 0x20;
          _Size = _Size - 0x80;
        } while ((_Size & 0xffffff00) != 0);
        goto LAB_00544830;
      }
    }
    if ((DAT_005b2618 >> 1 & 1) != 0) {
LAB_00544830:
      if (0x1f < _Size) {
        do {
          *piVar3 = iVar1;
          piVar3[1] = iVar1;
          piVar3[2] = iVar1;
          piVar3[3] = iVar1;
          piVar3[4] = iVar1;
          piVar3[5] = iVar1;
          piVar3[6] = iVar1;
          piVar3[7] = iVar1;
          piVar3 = piVar3 + 8;
          _Size = _Size - 0x20;
        } while (0x1f < _Size);
        if ((_Size & 0x1f) == 0) {
          return _Dst;
        }
      }
      piVar3 = (int *)((int)piVar3 + (_Size - 0x20));
      *piVar3 = iVar1;
      piVar3[1] = iVar1;
      piVar3[2] = iVar1;
      piVar3[3] = iVar1;
      piVar3[4] = iVar1;
      piVar3[5] = iVar1;
      piVar3[6] = iVar1;
      piVar3[7] = iVar1;
      return _Dst;
    }
  }
joined_r0x0054486b:
  for (; (_Size & 3) != 0; _Size = _Size - 1) {
    *(char *)piVar3 = (char)iVar1;
    piVar3 = (int *)((int)piVar3 + 1);
  }
  if ((_Size & 4) != 0) {
    *piVar3 = iVar1;
    piVar3 = piVar3 + 1;
    _Size = _Size - 4;
  }
  for (; (_Size & 0xfffffff8) != 0; _Size = _Size - 8) {
    *piVar3 = iVar1;
    piVar3[1] = iVar1;
    piVar3 = piVar3 + 2;
  }
  return _Dst;
}

