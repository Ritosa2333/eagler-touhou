/* Entry: 0x00498210; symbol: FUN_00498210; body bytes: 818 */

default_delete<class_std::_Facet_base> * __thiscall FUN_00498210(void *this,undefined4 param_1)

{
  void *pvVar1;
  int iVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  default_delete<class_std::_Facet_base> *pdVar4;
  
  switch(param_1) {
  case 0xffffd8f5:
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0048bd10((void *)((int)this + 0x1e0),0);
    pdVar4 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(p_Var3);
    break;
  case 0xffffd8f6:
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0048bd10((void *)((int)this + 0x1e0),0);
    pdVar4 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(p_Var3);
    pdVar4 = pdVar4 + 4;
    break;
  case 0xffffd8f7:
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0048bd10((void *)((int)this + 0x1e0),1);
    pdVar4 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(p_Var3);
    break;
  case 0xffffd8f8:
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0048bd10((void *)((int)this + 0x1e0),1);
    pdVar4 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(p_Var3);
    pdVar4 = pdVar4 + 4;
    break;
  default:
    pdVar4 = (default_delete<class_std::_Facet_base> *)0x0;
    break;
  case 0xffffd903:
    pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x110);
    break;
  case 0xffffd904:
    pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x114);
    break;
  case 0xffffd905:
    pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x118);
    break;
  case 0xffffd906:
    pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x11c);
    break;
  case 0xffffd92d:
    pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar2 = FUN_00485660(pvVar1,0);
    if (iVar2 == 0) {
      pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x110);
    }
    else {
      pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar2 = FUN_00485660(pvVar1,0);
      pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x110);
    }
    break;
  case 0xffffd92e:
    pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar2 = FUN_00485660(pvVar1,0);
    if (iVar2 == 0) {
      pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x114);
    }
    else {
      pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar2 = FUN_00485660(pvVar1,0);
      pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x114);
    }
    break;
  case 0xffffd92f:
    pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar2 = FUN_00485660(pvVar1,0);
    if (iVar2 == 0) {
      pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x118);
    }
    else {
      pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar2 = FUN_00485660(pvVar1,0);
      pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x118);
    }
    break;
  case 0xffffd930:
    pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar2 = FUN_00485660(pvVar1,0);
    if (iVar2 == 0) {
      pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x11c);
    }
    else {
      pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar2 = FUN_00485660(pvVar1,0);
      pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x11c);
    }
    break;
  case 0xffffd931:
    pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x120);
    break;
  case 0xffffd932:
    pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x124);
    break;
  case 0xffffd933:
    pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 0x128);
    break;
  case 0xffffd934:
    pdVar4 = (default_delete<class_std::_Facet_base> *)((int)this + 300);
    break;
  case 0xffffd93e:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x20);
    break;
  case 0xffffd93f:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x24);
    break;
  case 0xffffd940:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x28);
    break;
  case 0xffffd941:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x2c);
    break;
  case 0xffffd942:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x30);
    break;
  case 0xffffd943:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x34);
    break;
  case 0xffffd944:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x38);
    break;
  case 0xffffd945:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    pdVar4 = (default_delete<class_std::_Facet_base> *)(iVar2 + 0x3c);
  }
  return pdVar4;
}

