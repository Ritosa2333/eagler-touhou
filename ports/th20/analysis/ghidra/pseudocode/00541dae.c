/* Entry: 0x00541dae; symbol: __Mbrtowc; body bytes: 306 */

/* Library Function - Single Match
    __Mbrtowc
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

int __cdecl
__Mbrtowc(wchar_t *param_1,char *param_2,size_t param_3,mbstate_t *param_4,_Cvtvec *param_5)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  __acrt_ptd *p_Var4;
  undefined4 in_ECX;
  ushort uVar5;
  ushort extraout_DX;
  uint uVar6;
  uint uVar7;
  uint local_8;
  
  if (param_3 == 0) {
    return 0;
  }
  bVar1 = *param_2;
  uVar5 = (ushort)bVar1;
  if (bVar1 == 0) {
    *param_1 = L'\0';
    return 0;
  }
  local_8 = CONCAT31((int3)((uint)in_ECX >> 8),bVar1);
  if (param_5->_Isclocale != 0) {
LAB_00541ecf:
    *param_1 = uVar5 & 0xff;
    return 1;
  }
  uVar6 = param_5->_Mbcurmax;
  if (uVar6 == 1) {
LAB_00541ea8:
    iVar2 = 1;
    iVar3 = MultiByteToWideChar(param_5->_Page,9,param_2,1,param_1,1);
  }
  else {
    if (uVar6 != 2) {
      if (uVar6 == 4) {
        iVar2 = __Utf8_trailing_byte_count(&local_8,bVar1);
        uVar5 = extraout_DX;
        if (iVar2 == 0) goto LAB_00541ecf;
        if (iVar2 < 3) {
          uVar7 = 1;
          uVar6 = local_8;
          do {
            if (param_3 <= uVar7) {
              return -2;
            }
            if ((param_2[uVar7] & 0xc0U) != 0x80) goto LAB_00541ebf;
            uVar6 = uVar6 << 6 | (byte)param_2[uVar7] & 0x3f;
            uVar7 = uVar7 + 1;
            iVar2 = iVar2 + -1;
          } while (iVar2 != 0);
          if (0x7ff < uVar6 - 0xd800) {
            *param_1 = (wchar_t)uVar6;
            return uVar7;
          }
        }
        goto LAB_00541ebf;
      }
      goto LAB_00541ea8;
    }
    if ((param_5->_Isleadbyte[bVar1 >> 3] & (byte)(1 << (bVar1 & 7))) == 0) {
      iVar2 = 1;
    }
    else {
      if (param_3 == 1) {
        return -2;
      }
      iVar2 = 2;
    }
    iVar3 = MultiByteToWideChar(param_5->_Page,9,param_2,iVar2,param_1,1);
  }
  if (iVar3 != 0) {
    return iVar2;
  }
LAB_00541ebf:
  p_Var4 = FUN_005516c1();
  *(undefined4 *)p_Var4 = 0x2a;
  return -1;
}

