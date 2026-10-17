/* Entry: 0x0045a580; symbol: FUN_0045a580; body bytes: 1310 */

void __thiscall FUN_0045a580(void *this,int param_1)

{
  char **ppcVar1;
  undefined4 uVar2;
  int iVar3;
  int *unaff_FS_OFFSET;
  int local_54;
  int local_50;
  int local_4c;
  uint local_48;
  int local_3c;
  uint *local_38;
  uint local_34;
  undefined1 local_30 [4];
  undefined1 local_2c [4];
  int local_28;
  undefined4 local_24;
  uint local_20;
  uint local_1c;
  uint local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056884d;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  local_38 = (uint *)0x0;
  local_3c = 0;
  local_14 = ppcVar1;
  if ((*(int *)((int)this + 8) == 0) || (*(int *)((int)this + 0x10) == 0)) goto LAB_0045aa83;
  uVar2 = FUN_0040c310(&DAT_005c0240,0xc);
  FUN_0040b730(&local_24,uVar2);
  local_8 = 0;
  (**(code **)(*(int *)**(undefined4 **)((int)this + 8) + 0x10))
            (**(undefined4 **)((int)this + 8),local_2c,&local_20);
  if (((*(uint *)((int)this + 0x90) < local_20 - *(int *)((int)this + 0x9c)) ||
      (local_20 <= *(uint *)((int)this + 0x90))) &&
     ((-1 < (int)(local_20 - *(int *)((int)this + 0x9c)) ||
      (*(uint *)((int)this + 0x90) < (uint)(*(int *)((int)this + 0xc) - *(int *)((int)this + 0x9c)))
      ))) {
    iVar3 = FUN_0045b780(this,(int *)**(undefined4 **)((int)this + 8),&local_28);
    if (iVar3 < 0) {
      std::_Adl_verify_range<char*,char_const*>
                ((char **)"error : RestoreBuffer in HandleWaveStreamNotification\r\n",ppcVar1);
    }
    else if (local_28 == 0) {
      iVar3 = (**(code **)(*(int *)**(undefined4 **)((int)this + 8) + 0x2c))
                        (**(undefined4 **)((int)this + 8),*(undefined4 *)((int)this + 0x90),
                         *(undefined4 *)((int)this + 0x9c),&local_38,&local_18,&local_3c,local_30,0)
      ;
      if (iVar3 < 0) {
        std::_Adl_verify_range<char*,char_const*>
                  ((char **)"error : Buffer->Lock in HandleWaveStreamNotification\r\n",ppcVar1);
      }
      else {
        if (local_3c != 0) {
          local_8 = 0xffffffff;
          FUN_0040b900(&local_24);
          goto LAB_0045aa83;
        }
        if (*(int *)((int)this + 0x98) == 0) {
          iVar3 = FUN_0045aff0(*(void **)((int)this + 0x10),local_38,local_18,&local_34);
          if (iVar3 < 0) {
            std::_Adl_verify_range<char*,char_const*>
                      ((char **)"error : m_pWaveFile->Read in HandleWaveStreamNotification\r\n",
                       ppcVar1);
            goto LAB_0045aa65;
          }
        }
        else {
          if (*(short *)(*(int *)(*(int *)((int)this + 0x10) + 0x90) + 0x2e) == 8) {
            local_4c = 0x80;
          }
          else {
            local_4c = 0;
          }
          _memset(local_38,local_4c,local_18);
          local_34 = local_18;
        }
        if (local_34 < local_18) {
          *(undefined4 *)(*(int *)((int)this + 0x10) + 0x9c) = 1;
          if (param_1 == 0) {
            if (*(short *)(*(int *)(*(int *)((int)this + 0x10) + 0x90) + 0x2e) == 8) {
              local_50 = 0x80;
            }
            else {
              local_50 = 0;
            }
            _memset((void *)((int)local_38 + local_34),local_50,local_18 - local_34);
            *(undefined4 *)((int)this + 0x98) = 1;
          }
          else {
            for (local_48 = local_34; local_48 < local_18; local_48 = local_48 + local_34) {
              iVar3 = FUN_0045b5f0(*(void **)((int)this + 0x10),'\x01',0);
              if (iVar3 < 0) {
                std::_Adl_verify_range<char*,char_const*>
                          ((char **)
                           "error : m_pWaveFile->ResetFile in HandleWaveStreamNotification\r\n",
                           ppcVar1);
                goto LAB_0045aa65;
              }
              iVar3 = FUN_0045aff0(*(void **)((int)this + 0x10),(uint *)((int)local_38 + local_48),
                                   local_18 - local_48,&local_34);
              if (iVar3 < 0) {
                std::_Adl_verify_range<char*,char_const*>
                          ((char **)
                           "error : m_pWaveFile->Read(+) in HandleWaveStreamNotification\r\n",
                           ppcVar1);
                goto LAB_0045aa65;
              }
            }
          }
        }
        (**(code **)(*(int *)**(undefined4 **)((int)this + 8) + 0x4c))
                  (**(undefined4 **)((int)this + 8),local_38,local_18,0,0);
        iVar3 = (**(code **)(*(int *)**(undefined4 **)((int)this + 8) + 0x10))
                          (**(undefined4 **)((int)this + 8),&local_1c,0);
        if (iVar3 < 0) {
          std::_Adl_verify_range<char*,char_const*>
                    ((char **)
                     "error : m_apDSBuffer[0]->GetCurrentPosition in HandleWaveStreamNotification\r\n"
                     ,ppcVar1);
        }
        else {
          if (local_1c < *(uint *)((int)this + 0x88)) {
            local_54 = *(int *)((int)this + 0xc) - *(int *)((int)this + 0x88);
          }
          else {
            local_54 = -*(int *)((int)this + 0x88);
          }
          local_54 = local_54 + local_1c;
          *(int *)((int)this + 0x8c) = *(int *)((int)this + 0x8c) + local_54;
          *(uint *)((int)this + 0x88) = local_1c;
          *(uint *)((int)this + 0x90) = *(int *)((int)this + 0x90) + local_18;
          *(uint *)((int)this + 0x90) = *(uint *)((int)this + 0x90) % *(uint *)((int)this + 0xc);
        }
      }
    }
    else {
      iVar3 = FUN_0045a240(this,(int *)**(undefined4 **)((int)this + 8),0,0);
      if (iVar3 < 0) {
        std::_Adl_verify_range<char*,char_const*>
                  ((char **)"error : FillBufferWithSound in HandleWaveStreamNotification\r\n",
                   ppcVar1);
      }
    }
  }
  else {
    std::_Adl_verify_range<char*,char_const*>((char **)"Stream Skip\n",ppcVar1);
  }
LAB_0045aa65:
  local_8 = 0xffffffff;
  FUN_0040b900(&local_24);
LAB_0045aa83:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

