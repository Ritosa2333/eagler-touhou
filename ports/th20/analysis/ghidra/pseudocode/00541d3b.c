/* Entry: 0x00541d3b; symbol: __Getcvt; body bytes: 115 */

/* Library Function - Single Match
    __Getcvt
   
   Library: Visual Studio 2019 Release */

_Cvtvec * __cdecl __Getcvt(_Cvtvec *__return_storage_ptr__)

{
  wchar_t *pwVar1;
  UINT UVar2;
  uint uVar3;
  wchar_t **ppwVar4;
  ushort *puVar5;
  uint uVar6;
  
  uVar6 = 0;
  _memset(__return_storage_ptr__,0,0x2c);
  UVar2 = ____lc_codepage_func();
  __return_storage_ptr__->_Page = UVar2;
  uVar3 = ____mb_cur_max_func();
  __return_storage_ptr__->_Mbcurmax = uVar3;
  ppwVar4 = ____lc_locale_name_func();
  pwVar1 = ppwVar4[2];
  __return_storage_ptr__->_Isclocale = (uint)(pwVar1 == (wchar_t *)0x0);
  if (pwVar1 != (wchar_t *)0x0) {
    puVar5 = ___pctype_func();
    do {
      if ((short)puVar5[uVar6] < 0) {
        __return_storage_ptr__->_Isleadbyte[uVar6 >> 3] =
             __return_storage_ptr__->_Isleadbyte[uVar6 >> 3] | (byte)(1 << (uVar6 & 7));
      }
      uVar6 = uVar6 + 1;
    } while ((int)uVar6 < 0x100);
  }
  return __return_storage_ptr__;
}

