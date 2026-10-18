/* Entry: 0x00426d70; symbol: FUN_00426d70; body bytes: 320 */

void __thiscall FUN_00426d70(void *this,int param_1,undefined4 param_2)

{
  short sVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int *piVar3;
  int iVar4;
  Image *this_00;
  default_delete<class_std::_Facet_base> *local_14;
  uint local_c;
  
  sVar1 = *(short *)(&DAT_005ae6f2 + param_1 * 0x14);
  local_c = 0;
  local_14 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)this + 0x1c));
  pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_00428c40((int)this + 0x1c);
  while( true ) {
    if ((local_14 == pdVar2) || (*(int *)local_14 < 0)) {
      if ((int)local_c < 0xc) {
        piVar3 = (int *)FUN_00428e20((void *)((int)this + 0x1c),local_c);
        *piVar3 = param_1;
        iVar4 = FUN_004260b0((void *)((int)this + 0x1c),local_c);
        *(undefined4 *)(iVar4 + 8) = param_2;
        iVar4 = FUN_004260b0((void *)((int)this + 0x1c),local_c);
        *(undefined4 *)(iVar4 + 4) = 1;
        this_00 = (Image *)FUN_004260f0((void *)((int)this + 0x1994),param_1);
        Gdiplus::Image::SetNativeImage(this_00,(GpImage *)(int)sVar1);
      }
      return;
    }
    if (*(int *)local_14 == param_1) break;
    local_c = local_c + 1;
    local_14 = local_14 + 0x208;
  }
  if (0x3b < *(int *)(local_14 + 4)) {
    return;
  }
  if (*(int *)(local_14 + 4) < 0) {
    return;
  }
  *(undefined4 *)(local_14 + *(int *)(local_14 + 4) * 4 + 8) = param_2;
  *(int *)(local_14 + 4) = *(int *)(local_14 + 4) + 1;
  return;
}

