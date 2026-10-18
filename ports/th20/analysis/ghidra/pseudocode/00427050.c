/* Entry: 0x00427050; symbol: FUN_00427050; body bytes: 754 */

void __thiscall FUN_00427050(void *this,int param_1,undefined1 (*param_2) [16])

{
  code *pcVar1;
  char *_Str1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  uint *puVar5;
  int *piVar6;
  undefined4 *puVar7;
  undefined4 local_54;
  undefined4 local_50;
  int local_4c;
  undefined4 *local_44;
  uint *local_30;
  uint *local_2c;
  uint local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined2 local_14;
  uint local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  _Str1 = (char *)FUN_00410aa0(param_2,(SIZE_T *)0x0,0);
  if (_Str1 != (char *)0x0) {
    iVar2 = _strncmp(_Str1,"RIFF",4);
    if (iVar2 == 0) {
      iVar2 = *(int *)(_Str1 + 4);
      iVar3 = _strncmp(_Str1 + 8,"WAVE",4);
      if (iVar3 == 0) {
        pcVar4 = FUN_004283b0(_Str1 + 0xc,"fmt ",&local_c,iVar2 + -0xc);
        if (pcVar4 != (char *)0x0) {
          local_24 = *(undefined4 *)pcVar4;
          local_20 = *(undefined4 *)(pcVar4 + 4);
          local_1c = *(undefined4 *)(pcVar4 + 8);
          local_18 = *(undefined4 *)(pcVar4 + 0xc);
          local_14 = *(undefined2 *)(pcVar4 + 0x10);
          puVar5 = (uint *)FUN_004283b0(_Str1 + 0xc,"data",&local_c,iVar2 + -0xc);
          if (puVar5 != (uint *)0x0) {
            _memset(&local_54,0,0x24);
            local_54 = 0x24;
            local_50 = 0x80c8;
            local_4c = local_c;
            local_44 = &local_24;
            piVar6 = (int *)FUN_0040c300((undefined4 *)&DAT_005ba830);
            pcVar1 = *(code **)(*piVar6 + 0xc);
            iVar2 = FUN_00414580((void *)((int)this + 0x2204),param_1);
            iVar2 = (*pcVar1)(piVar6,&local_54,iVar2,0);
            if (-1 < iVar2) {
              puVar7 = (undefined4 *)FUN_00414580((void *)((int)this + 0x2204),param_1);
              iVar2 = (**(code **)(*(int *)*puVar7 + 0x2c))
                                ((int *)*puVar7,0,local_c,&local_30,&local_28,&local_2c,&local_10,0)
              ;
              if (-1 < iVar2) {
                FUN_00543e20(local_30,puVar5,local_28);
                if (local_10 != 0) {
                  FUN_00543e20(local_2c,(uint *)((int)puVar5 + local_28),local_10);
                }
                puVar7 = (undefined4 *)FUN_00414580((void *)((int)this + 0x2204),param_1);
                (**(code **)(*(int *)*puVar7 + 0x4c))
                          ((int *)*puVar7,local_30,local_28,local_2c,local_10);
                if (_Str1 != (char *)0x0) {
                  FUN_0041f670(_Str1);
                }
                std::_Adl_verify_range<char*,char_const*>
                          ((char **)"Create Sound Buffer %s\n",(char **)param_2);
                goto LAB_00427350;
              }
            }
          }
        }
      }
    }
    if (_Str1 != (char *)0x0) {
      FUN_0041f670(_Str1);
    }
  }
LAB_00427350:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

