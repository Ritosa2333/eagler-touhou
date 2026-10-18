/* Entry: 0x004c1030; symbol: FUN_004c1030; body bytes: 2696 */

uint __thiscall FUN_004c1030(void *this,float *param_1,float *param_2,float param_3,float param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  int iVar10;
  char cVar11;
  default_delete<class_std::_Facet_base> *pdVar12;
  void *extraout_EAX;
  float10 fVar13;
  
  if ((*(uint *)((int)this + 0x14) >> 1 & 7) == 0) {
    if (param_2 == (float *)0x0) {
      fVar1 = param_1[1];
      fVar2 = *param_1;
      fVar13 = FUN_004292e0((float *)((int)this + 0x24));
      fVar3 = *(float *)((int)this + 0x30);
      fVar4 = *(float *)((int)this + 0x2c);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar5 = *(float *)(pdVar12 + 4);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      this = (void *)FUN_00457610(*(float *)pdVar12,fVar5,fVar4,fVar3,(float)fVar13,fVar2,fVar1,
                                  param_4);
      if (((uint)this & 0xff) != 0) {
        return CONCAT31((int3)((uint)this >> 8),1);
      }
    }
    else {
      fVar1 = param_2[1];
      fVar2 = *param_2;
      fVar3 = param_1[1];
      fVar4 = *param_1;
      fVar13 = FUN_004292e0((float *)((int)this + 0x24));
      fVar5 = *(float *)((int)this + 0x30);
      fVar6 = *(float *)((int)this + 0x2c);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar7 = *(float *)(pdVar12 + 4);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      this = (void *)FUN_004580c0(*(float *)pdVar12,fVar7,fVar6,fVar5,(float)fVar13,fVar4,fVar3,
                                  fVar2,fVar1,param_3);
      if (((uint)this & 0xff) != 0) {
        return CONCAT31((int3)((uint)this >> 8),1);
      }
    }
  }
  else if ((*(uint *)((int)this + 0x14) >> 1 & 7) == 1) {
    if (param_2 == (float *)0x0) {
      fVar1 = *(float *)((int)this + 0x18);
      fVar2 = *(float *)((int)this + 0x18);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar13 = FUN_004560d0((float *)pdVar12,param_1);
      this = extraout_EAX;
      if ((float)fVar13 <= (fVar1 + param_4) * (fVar2 + param_4)) {
        return CONCAT31((int3)((uint)extraout_EAX >> 8),1);
      }
    }
    else {
      fVar1 = *(float *)((int)this + 0x18);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar2 = *(float *)(pdVar12 + 4);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      cVar11 = FUN_00457610(*param_1,param_1[1],*param_2,param_2[1],param_3,*(float *)pdVar12,fVar2,
                            fVar1);
      if (cVar11 != '\0') {
        return 1;
      }
      this = (void *)0x0;
    }
  }
  else if ((*(uint *)((int)this + 0x14) >> 1 & 7) == 2) {
    if (param_2 == (float *)0x0) {
      fVar13 = FUN_004292e0((float *)((int)this + 0x24));
      fVar1 = *(float *)((int)this + 0x30);
      fVar2 = *(float *)((int)this + 0x2c);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar3 = *(float *)(pdVar12 + 4);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      this = (void *)FUN_004562e0(*param_1,param_1[1],param_4,*(float *)pdVar12,fVar3,fVar2,fVar1,
                                  (float)fVar13);
      if (((uint)this & 0xff) != 0) {
        return CONCAT31((int3)((uint)this >> 8),1);
      }
    }
    else {
      fVar13 = FUN_004292e0((float *)((int)this + 0x24));
      fVar1 = *(float *)((int)this + 0x30);
      fVar2 = *(float *)((int)this + 0x2c);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar3 = *(float *)(pdVar12 + 4);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      cVar11 = FUN_004578b0(*param_1,param_1[1],*param_2,param_2[1],param_3,*(float *)pdVar12,fVar3,
                            fVar2,fVar1,(float)fVar13);
      if (cVar11 != '\0') {
        return 1;
      }
      this = (void *)0x0;
    }
  }
  else if ((*(uint *)((int)this + 0x14) >> 1 & 7) == 3) {
    if (param_2 == (float *)0x0) {
      iVar10 = *(int *)((int)this + 0xb8);
      fVar13 = FUN_004292e0((float *)((int)this + 0x24));
      uVar8 = *(undefined4 *)((int)this + 0x18);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar1 = *(float *)(pdVar12 + 4);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      this = (void *)FUN_00456690(*param_1,param_1[1],param_4,*(float *)pdVar12,fVar1,uVar8,
                                  (float)fVar13,iVar10);
      if (((uint)this & 0xff) != 0) {
        return CONCAT31((int3)((uint)this >> 8),1);
      }
    }
    else {
      iVar10 = *(int *)((int)this + 0xb8);
      fVar13 = FUN_004292e0((float *)((int)this + 0x24));
      uVar8 = *(undefined4 *)((int)this + 0x18);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar1 = *(float *)(pdVar12 + 4);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      this = (void *)FUN_00457c10(*param_1,param_1[1],*param_2,param_2[1],param_3,*(float *)pdVar12,
                                  fVar1,uVar8,(float)fVar13,iVar10);
      if (((uint)this & 0xff) != 0) {
        return CONCAT31((int3)((uint)this >> 8),1);
      }
    }
  }
  else if ((*(uint *)((int)this + 0x14) >> 1 & 7) == 4) {
    if (param_2 == (float *)0x0) {
      iVar10 = *(int *)((int)this + 0xb8);
      fVar13 = FUN_004292e0((float *)((int)this + 0x24));
      uVar8 = *(undefined4 *)((int)this + 0x1c);
      uVar9 = *(undefined4 *)((int)this + 0x18);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar1 = *(float *)(pdVar12 + 4);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      this = (void *)FUN_004567d0(*param_1,param_1[1],param_4,*(float *)pdVar12,fVar1,uVar9,uVar8,
                                  (float)fVar13,iVar10);
      if (((uint)this & 0xff) != 0) {
        return CONCAT31((int3)((uint)this >> 8),1);
      }
    }
    else {
      iVar10 = *(int *)((int)this + 0xb8);
      fVar13 = FUN_004292e0((float *)((int)this + 0x24));
      uVar8 = *(undefined4 *)((int)this + 0x1c);
      uVar9 = *(undefined4 *)((int)this + 0x18);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      fVar1 = *(float *)(pdVar12 + 4);
      pdVar12 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)((int)this + 0x34));
      this = (void *)FUN_00458670(*param_1,param_1[1],*param_2,param_2[1],param_3,*(float *)pdVar12,
                                  fVar1,uVar9,uVar8,(float)fVar13,iVar10);
      if (((uint)this & 0xff) != 0) {
        return CONCAT31((int3)((uint)this >> 8),1);
      }
    }
  }
  return (uint)this & 0xffffff00;
}

