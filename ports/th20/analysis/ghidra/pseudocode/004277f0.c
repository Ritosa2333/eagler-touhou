/* Entry: 0x004277f0; symbol: FUN_004277f0; body bytes: 2909 */

void __fastcall FUN_004277f0(void *param_1)

{
  bool bVar1;
  char **ppcVar2;
  undefined4 uVar3;
  uint uVar4;
  DWORD DVar5;
  int iVar6;
  default_delete<class_std::_Facet_base> *pdVar7;
  int *piVar8;
  void *this;
  int *piVar9;
  int *unaff_FS_OFFSET;
  uint local_54;
  undefined1 (*local_4c) [16];
  uint local_48;
  int local_3c;
  default_delete<class_std::_Facet_base> *local_38;
  int local_34;
  int local_30;
  int *local_28;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568215;
  local_10 = *unaff_FS_OFFSET;
  ppcVar2 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = ppcVar2;
  uVar3 = FUN_0040c310(&DAT_005c0240,0xb);
  FUN_0040b730(&local_1c,uVar3);
  if (*(int *)((int)param_1 + 0xc) != 0) {
    FUN_0040b900(&local_1c);
    local_28 = (int *)FUN_004260d0((void *)((int)param_1 + 0x2544),0);
LAB_0042787e:
    uVar3 = FUN_0040c310(&DAT_005c0240,0xb);
    FUN_0040b730(&local_18,uVar3);
    local_8 = 0;
    bVar1 = false;
    switch(*local_28) {
    case 1:
      uVar4 = FUN_00428fa0(0x5c4d40);
      if (uVar4 == 0) {
        std::_Adl_verify_range<char*,char_const*>((char **)"Sound : PreLoad Stage\r\n",ppcVar2);
        FUN_00427360(param_1,(char **)local_28[1],(undefined1 (*) [16])(local_28 + 3));
        bVar1 = true;
      }
      else {
        std::_Adl_verify_range<char*,char_const*>((char **)"Sound : PreLoad Stage\r\n",ppcVar2);
        if (local_28[2] != 0) {
          local_28[2] = local_28[2] + 1;
          local_8 = 0xffffffff;
          FUN_0040b900(&local_18);
          goto LAB_0042818a;
        }
        FUN_00428990((int)param_1);
        FUN_00427360(param_1,(char **)local_28[1],(undefined1 (*) [16])(local_28 + 3));
        bVar1 = true;
      }
      break;
    case 2:
      uVar4 = FUN_00428fa0(0x5c4d40);
      if ((uVar4 == 0) || (local_28[1] < 0)) {
        if (*(int *)((int)param_1 + 0x57c4) != 0) {
          if (local_28[2] == 0) {
            std::_Adl_verify_range<char*,char_const*>((char **)"Sound : Stop Stage\r\n",ppcVar2);
            FUN_0045bb00(*(void **)((int)param_1 + 0x57c4),0);
          }
          else {
            if (local_28[2] == 1) {
              if (*(int *)(*(int *)((int)param_1 + 0x57c4) + 0xa4) == 0) {
                if (local_28[1] < 0) {
                  local_4c = (undefined1 (*) [16])(local_28 + 3);
                }
                else {
                  local_4c = (undefined1 (*) [16])((int)param_1 + local_28[1] * 0x100 + 0x46c4);
                }
                FUN_00548610((char *)((int)param_1 + 0x2444),0x100,(int)local_4c);
                iVar6 = FUN_00428420(param_1,local_4c);
                local_28[1] = iVar6;
                std::_Adl_verify_range<char*,char_const*>
                          ((char **)"Sound : Recreate Stage\r\n",ppcVar2);
                FUN_0045b150(*(void **)((int)param_1 + 0x57c4),
                             local_28[1] * 0x34 + *(int *)((int)param_1 + 0x1890));
                goto LAB_00427d83;
              }
              local_8 = 0xffffffff;
              FUN_0040b900(&local_18);
              goto LAB_0042818a;
            }
            if (local_28[2] == 2) {
              std::_Adl_verify_range<char*,char_const*>((char **)"Sound : ReOpen Stage\r\n",ppcVar2)
              ;
              FUN_0045af10(*(void **)((int)param_1 + 0x57c4),
                           (char **)(local_28[1] * 0x34 + *(int *)((int)param_1 + 0x1890)),0);
            }
            else {
              if (local_28[2] == 3) {
                std::_Adl_verify_range<char*,char_const*>
                          ((char **)"Sound : Fill Buffer Stage\r\n",ppcVar2);
                piVar8 = (int *)FUN_0045a460(*(void **)((int)param_1 + 0x57c4),0);
                (**(code **)(**(int **)((int)param_1 + 0x57c4) + 4))(0);
                iVar6 = FUN_00411700(*(int *)((int)param_1 + 0x57c4));
                iVar6 = FUN_00426370(iVar6);
                local_54 = (uint)(*(int *)(iVar6 + 0x1c) != 0);
                local_28[1] = local_54;
                iVar6 = FUN_0045a240(*(void **)((int)param_1 + 0x57c4),piVar8,local_28[1],0);
                goto joined_r0x00427d33;
              }
              if (local_28[2] == 4) {
                std::_Adl_verify_range<char*,char_const*>((char **)"Sound : Play Stage\r\n",ppcVar2)
                ;
                FUN_0045add0(*(void **)((int)param_1 + 0x57c4),0,1,0);
              }
              else if (6 < local_28[2]) break;
            }
          }
          goto LAB_00427d83;
        }
      }
      else {
        if (local_28[2] != 0) {
          if (local_28[2] == 2) {
            std::_Adl_verify_range<char*,char_const*>((char **)"Sound : Reset Stage\r\n",ppcVar2);
            if (*(int *)((int)param_1 + 0x57c4) != 0) {
              iVar6 = (**(code **)(**(int **)((int)param_1 + 0x57c4) + 4))(0);
joined_r0x00427d33:
              if (iVar6 < 0) break;
            }
          }
          else {
            if (local_28[2] == 5) {
              std::_Adl_verify_range<char*,char_const*>
                        ((char **)"Sound : Fill Buffer Stage\r\n",ppcVar2);
              piVar8 = (int *)FUN_0045a460(*(void **)((int)param_1 + 0x57c4),0);
              iVar6 = FUN_00411700(*(int *)((int)param_1 + 0x57c4));
              iVar6 = FUN_00426370(iVar6);
              local_48 = (uint)(*(int *)(iVar6 + 0x1c) != 0);
              local_28[1] = local_48;
              iVar6 = FUN_0045a240(*(void **)((int)param_1 + 0x57c4),piVar8,local_28[1],0);
              goto joined_r0x00427d33;
            }
            if (local_28[2] == 7) {
              std::_Adl_verify_range<char*,char_const*>((char **)"Sound : Play Stage\r\n",ppcVar2);
              FUN_0045add0(*(void **)((int)param_1 + 0x57c4),0,1,0);
            }
            else if (0x13 < local_28[2]) break;
          }
LAB_00427d83:
          local_28[2] = local_28[2] + 1;
          local_8 = 0xffffffff;
          FUN_0040b900(&local_18);
          goto LAB_0042818a;
        }
        std::_Adl_verify_range<char*,char_const*>((char **)"Sound : Load Stage\r\n",ppcVar2);
        iVar6 = FUN_00426890(param_1,(char **)local_28[1]);
        if (iVar6 == 0) goto LAB_00427d83;
      }
      break;
    case 3:
      if (*(int *)((int)param_1 + 0x57c4) != 0) {
        if (local_28[2] == 0) {
          std::_Adl_verify_range<char*,char_const*>((char **)"Sound : Stop Stage\r\n",ppcVar2);
          FUN_0045bb00(*(void **)((int)param_1 + 0x57c4),1);
        }
        else if (local_28[2] == 1) break;
        local_28[2] = local_28[2] + 1;
        local_8 = 0xffffffff;
        FUN_0040b900(&local_18);
        goto LAB_0042818a;
      }
      break;
    case 4:
      if (*(int *)((int)param_1 + 0x57c4) != 0) {
        if (local_28[2] == 0) {
          std::_Adl_verify_range<char*,char_const*>((char **)"Sound : Stop Stage\r\n",ppcVar2);
          FUN_0045bb00(*(void **)((int)param_1 + 0x57c4),1);
        }
        else if (local_28[2] == 1) {
          std::_Adl_verify_range<char*,char_const*>
                    ((char **)"Sound : Thread Stop Stage\r\n",ppcVar2);
          if (*(int *)((int)param_1 + 0x14) == 0) break;
          PostThreadMessageW(*(DWORD *)((int)param_1 + 0x10),0x12,0,0);
        }
        else if (local_28[2] == 2) {
          DVar5 = WaitForSingleObject(*(HANDLE *)((int)param_1 + 0x14),0x100);
          if (DVar5 == 0) {
            *(undefined4 *)((int)param_1 + 0x14) = 0;
          }
          else {
            std::_Adl_verify_range<char*,char_const*>
                      ((char **)"Sound : Thread Stop Wait Stage\r\n",ppcVar2);
            PostThreadMessageW(*(DWORD *)((int)param_1 + 0x10),0x12,0,0);
            local_28[2] = local_28[2] + -1;
          }
        }
        else if (local_28[2] == 3) {
          std::_Adl_verify_range<char*,char_const*>
                    ((char **)"Sound : Handle Close Stage\r\n",ppcVar2);
          CloseHandle(*(HANDLE *)((int)param_1 + 0x14));
          CloseHandle(*(HANDLE *)((int)param_1 + 0x57cc));
          *(undefined4 *)((int)param_1 + 0x14) = 0;
          if (*(int *)((int)param_1 + 0x57c4) != 0) {
            FUN_0041f7c0(*(undefined4 **)((int)param_1 + 0x57c4));
            *(undefined4 *)((int)param_1 + 0x57c4) = 0;
          }
          *(undefined4 *)((int)param_1 + 0x57c4) = 0;
        }
        else if (local_28[2] == 10) break;
        local_28[2] = local_28[2] + 1;
        local_8 = 0xffffffff;
        FUN_0040b900(&local_18);
        goto LAB_0042818a;
      }
      break;
    case 5:
      std::_Adl_verify_range<char*,char_const*>
                ((char **)"Sound : Fade Out Stage %d\r\n",(char **)local_28[1]);
      FUN_00426330(&DAT_005ba830,(float)local_28[1]);
      break;
    case 6:
      if ((DAT_005c4f7d == '\x01') && (*(int *)((int)param_1 + 0x57c4) != 0)) {
        if (*(int *)(*(int *)((int)param_1 + 0x57c4) + 0xa4) != 0) {
          std::_Adl_verify_range<char*,char_const*>((char **)"locked\n",ppcVar2);
          local_8 = 0xffffffff;
          FUN_0040b900(&local_18);
          goto LAB_0042818a;
        }
        if (*(int *)((int)param_1 + 0x57c4) != 0) {
          FUN_0045acf0(*(int *)((int)param_1 + 0x57c4));
        }
      }
      break;
    case 7:
      if ((DAT_005c4f7d == '\x01') && (*(int *)((int)param_1 + 0x57c4) != 0)) {
        if (*(int *)(*(int *)((int)param_1 + 0x57c4) + 0xa4) != 0) {
          local_8 = 0xffffffff;
          FUN_0040b900(&local_18);
          goto LAB_0042818a;
        }
        if (*(int *)((int)param_1 + 0x57c4) != 0) {
          FUN_0045bbe0(*(void **)((int)param_1 + 0x57c4));
        }
      }
      break;
    case 8:
      if (*(int *)((int)param_1 + 0x57c4) != 0) {
        FUN_0045b9b0(*(void **)((int)param_1 + 0x57c4),0);
      }
      break;
    case 9:
      if (*(int *)((int)param_1 + 0x57c4) != 0) {
        iVar6 = FUN_00428420(param_1,(undefined1 (*) [16])(local_28 + 3));
        FUN_0045bab0(*(void **)((int)param_1 + 0x57c4),
                     iVar6 * 0x34 + *(int *)((int)param_1 + 0x1890));
      }
      break;
    default:
      local_8 = 0xffffffff;
      FUN_0040b900(&local_18);
      goto LAB_0042818a;
    }
    local_34 = 0;
    for (; (local_34 < 0x1f && (*local_28 != 0)); local_28 = local_28 + 0x43) {
      piVar8 = local_28 + 0x43;
      piVar9 = local_28;
      for (iVar6 = 0x43; iVar6 != 0; iVar6 = iVar6 + -1) {
        *piVar9 = *piVar8;
        piVar8 = piVar8 + 1;
        piVar9 = piVar9 + 1;
      }
      local_34 = local_34 + 1;
    }
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
    Sleep(1);
    if (!bVar1) goto LAB_0042818a;
    goto LAB_0042787e;
  }
  FUN_0040b900(&local_1c);
  goto LAB_00428332;
LAB_0042818a:
  if (DAT_005c4f7e == '\0') {
    FUN_004260d0((void *)((int)param_1 + 0x2544),0);
  }
  else {
    uVar3 = FUN_0040c310(&DAT_005c0240,0xb);
    FUN_0040b730(&local_20,uVar3);
    local_8 = 1;
    local_38 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)((int)param_1 + 0x1c));
    pdVar7 = (default_delete<class_std::_Facet_base> *)FUN_00428c40((int)param_1 + 0x1c);
    for (; (local_38 != pdVar7 && (-1 < *(int *)local_38)); local_38 = local_38 + 0x208) {
      iVar6 = *(int *)local_38;
      *(int *)local_38 = -1;
      if (*(int *)(local_38 + 4) < 0) {
        piVar8 = (int *)FUN_004260f0((void *)((int)param_1 + 0x1994),iVar6);
        FUN_00428810(piVar8);
        *(int *)(local_38 + 4) = 0;
      }
      else {
        local_30 = 0;
        for (local_3c = 0; local_3c < *(int *)(local_38 + 4); local_3c = local_3c + 1) {
          local_30 = local_30 + *(int *)(local_38 + local_3c * 4 + 8);
        }
        if (0 < *(int *)(local_38 + 4)) {
          local_30 = local_30 / *(int *)(local_38 + 4);
        }
        *(int *)(local_38 + 4) = 0;
        this = (void *)FUN_004260f0((void *)((int)param_1 + 0x1994),iVar6);
        FUN_00426ef0(this,local_30);
      }
    }
    local_8 = 0xffffffff;
    FUN_0040b900(&local_20);
    FUN_004260d0((void *)((int)param_1 + 0x2544),0);
  }
LAB_00428332:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

