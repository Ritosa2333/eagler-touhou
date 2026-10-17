/* Entry: 0x0046d300; symbol: FUN_0046d300; body bytes: 5637 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0046d300(void *this,byte *param_1)

{
  undefined4 uVar1;
  size_t sVar2;
  int iVar3;
  float10 fVar4;
  float fVar5;
  int local_98;
  int local_94;
  int local_84;
  int local_80;
  float local_74;
  float local_70;
  float local_6c;
  float local_68;
  float local_5c;
  float local_58;
  float local_54;
  float local_50;
  float local_48;
  float local_44;
  byte *local_40;
  float local_38;
  float local_34;
  undefined4 local_30;
  float local_2c;
  float local_28;
  float local_20;
  float local_1c;
  float local_14;
  float local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  sVar2 = _strlen((char *)param_1);
  FUN_00422dd0(&local_14,0,0,0);
  *(byte *)((int)this + 0x4aa) = *(byte *)((int)this + 0x4aa) | 1;
  *(undefined1 *)((int)this + 0x4b4) = 0;
  *(undefined4 *)((int)this + 0x4b8) = 1;
  *(undefined4 *)((int)this + 0x4bc) = 1;
  FUN_00470af0((void *)((int)this + 0x10),(undefined4 *)(param_1 + 0x100));
  FUN_0045dd40((void *)((int)this + 0x10),(char)*(undefined4 *)(param_1 + 0x110));
  FUN_00470e50((void *)((int)this + 0x10),*(undefined4 *)(param_1 + 0x114),
               *(undefined4 *)(param_1 + 0x118));
  switch(*(undefined4 *)(param_1 + 0x128)) {
  default:
    if (*(float *)(param_1 + 0x114) == DAT_0056c8cc) {
      *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3;
    }
    else {
      *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3 | 4;
    }
    local_44 = (float)*(int *)((int)this + 0x1a23c) * *(float *)(param_1 + 0x114);
    local_48 = DAT_0056f7ac * *(float *)(param_1 + 0x118);
    break;
  case 1:
    *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3;
    local_44 = DAT_0056f00c * *(float *)(param_1 + 0x114);
    local_48 = DAT_0056f7a4 * *(float *)(param_1 + 0x118);
    break;
  case 2:
    *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3;
    local_44 = DAT_0056f7a0 * *(float *)(param_1 + 0x114);
    local_48 = DAT_0056e734 * *(float *)(param_1 + 0x118);
    break;
  case 3:
    *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3 | 4;
    local_44 = DAT_0056f7a0 * *(float *)(param_1 + 0x114);
    local_48 = DAT_0056e734 * *(float *)(param_1 + 0x118);
    break;
  case 4:
  case 10:
  case 0xb:
    *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3;
    local_44 = DAT_0056f7a8 * *(float *)(param_1 + 0x114);
    local_48 = DAT_0056cd90 * *(float *)(param_1 + 0x118);
    break;
  case 5:
    *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3 | 4;
    local_44 = DAT_0056f7a8 * *(float *)(param_1 + 0x114);
    local_48 = DAT_0056cd90 * *(float *)(param_1 + 0x118);
    break;
  case 6:
  case 8:
    if (*(float *)(param_1 + 0x114) == DAT_0056c8cc) {
      *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3;
    }
    else {
      *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3 | 4;
    }
    local_44 = (DAT_0056f7b4 / DAT_0056c8d0) * *(float *)(param_1 + 0x114);
    local_48 = (DAT_0056cd94 / DAT_0056c8d0) * *(float *)(param_1 + 0x118);
    break;
  case 7:
  case 9:
    if (*(float *)(param_1 + 0x114) == DAT_0056c8cc) {
      *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3;
    }
    else {
      *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3 | 4;
    }
    local_44 = (DAT_0056f7b8 / DAT_0056c8d0) * *(float *)(param_1 + 0x114);
    local_48 = (DAT_0056f7bc / DAT_0056c8d0) * *(float *)(param_1 + 0x118);
    break;
  case 0xc:
  case 0xd:
    if (*(float *)(param_1 + 0x114) == DAT_0056c8cc) {
      *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3;
    }
    else {
      *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3 | 4;
    }
    local_44 = (DAT_0056f7ac / DAT_0056c8d0) * *(float *)(param_1 + 0x114);
    local_48 = (DAT_0056f7b0 / DAT_0056c8d0) * *(float *)(param_1 + 0x118);
  }
  if (*(int *)(param_1 + 0x138) == 0) {
    switch(*(undefined4 *)(param_1 + 0x128)) {
    case 2:
    case 3:
      local_50 = 0.0;
      for (local_40 = param_1; *local_40 != 0; local_40 = local_40 + 1) {
        if (*local_40 == 0x2e) {
          fVar5 = DAT_0056f7c0 * *(float *)(param_1 + 0x114);
        }
        else {
          fVar5 = (float)((uint)local_44 ^ _DAT_0056d7d0);
        }
        local_50 = (fVar5 / DAT_0056c8d0) * DAT_005b8818 + local_50;
      }
      local_14 = local_50;
      break;
    case 4:
    case 5:
    case 10:
    case 0xb:
      local_54 = 0.0;
      for (local_40 = param_1; *local_40 != 0; local_40 = local_40 + 1) {
        if (*local_40 == 0x2c) {
          fVar5 = DAT_0056f7c0 * *(float *)(param_1 + 0x114);
        }
        else {
          fVar5 = (float)((uint)local_44 ^ _DAT_0056d7d0);
        }
        local_54 = (fVar5 / DAT_0056c8d0) * DAT_005b8818 + local_54;
      }
      local_14 = local_54;
      break;
    case 6:
    case 7:
    case 8:
    case 9:
      if ((*(int *)(param_1 + 0x128) == 6) || (*(int *)(param_1 + 0x128) == 8)) {
        local_80 = 0x140;
      }
      else {
        local_80 = 0x204;
      }
      local_68 = 0.0;
      for (local_40 = param_1; *local_40 != 0; local_40 = local_40 + 1) {
        FUN_00470ed0((void *)((int)this + 0x10),(*local_40 - 0x20) + local_80);
        iVar3 = FUN_00437cd0((int)this + 0x10);
        local_68 = *(float *)(iVar3 + 0x4c) / (DAT_0056c8d8 - DAT_005b8818) + local_68;
      }
      local_14 = ((float)((uint)local_68 ^ _DAT_0056d7d0) * *(float *)(param_1 + 0x114)) /
                 DAT_0056c8d0;
      break;
    case 0xc:
    case 0xd:
      local_6c = 0.0;
      for (local_40 = param_1; *local_40 != 0; local_40 = local_40 + 1) {
        if (*local_40 < 0x20) {
          FUN_00470ed0((void *)((int)this + 0x10),0x349 - (uint)*local_40);
        }
        else {
          FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x2a8);
        }
        iVar3 = FUN_00437cd0((int)this + 0x10);
        local_6c = *(float *)(iVar3 + 0x4c) / (DAT_0056c8d8 - DAT_005b8818) + local_6c;
      }
      local_14 = ((float)((uint)local_6c ^ _DAT_0056d7d0) * *(float *)(param_1 + 0x114)) /
                 DAT_0056c8d0;
      break;
    default:
      local_14 = (((float)((uint)(float)(int)sVar2 ^ _DAT_0056d7d0) * local_44) / DAT_0056c8d0) *
                 DAT_005b8818;
    }
  }
  else if (*(int *)(param_1 + 0x138) == 2) {
    switch(*(undefined4 *)(param_1 + 0x128)) {
    case 2:
    case 3:
      local_58 = 0.0;
      for (local_40 = param_1; *local_40 != 0; local_40 = local_40 + 1) {
        if (*local_40 == 0x2e) {
          fVar5 = DAT_0056f7c0 * *(float *)(param_1 + 0x114);
        }
        else {
          fVar5 = (float)((uint)local_44 ^ _DAT_0056d7d0);
        }
        local_58 = fVar5 * DAT_005b8818 + local_58;
      }
      local_14 = local_58;
      break;
    case 4:
    case 5:
    case 10:
    case 0xb:
      local_5c = 0.0;
      for (local_40 = param_1; *local_40 != 0; local_40 = local_40 + 1) {
        if (*local_40 == 0x2c) {
          fVar5 = DAT_0056f7c0 * *(float *)(param_1 + 0x114);
        }
        else {
          fVar5 = (float)((uint)local_44 ^ _DAT_0056d7d0);
        }
        local_5c = fVar5 * DAT_005b8818 + local_5c;
      }
      local_14 = local_5c;
      break;
    case 6:
    case 7:
    case 8:
    case 9:
      if ((*(int *)(param_1 + 0x128) == 6) || (*(int *)(param_1 + 0x128) == 8)) {
        local_84 = 0x140;
      }
      else {
        local_84 = 0x204;
      }
      local_70 = 0.0;
      for (local_40 = param_1; *local_40 != 0; local_40 = local_40 + 1) {
        FUN_00470ed0((void *)((int)this + 0x10),(*local_40 - 0x20) + local_84);
        iVar3 = FUN_00437cd0((int)this + 0x10);
        local_70 = *(float *)(iVar3 + 0x4c) / (DAT_0056c8d8 - DAT_005b8818) + local_70;
      }
      local_14 = (float)((uint)local_70 ^ _DAT_0056d7d0) * *(float *)(param_1 + 0x114);
      break;
    case 0xc:
    case 0xd:
      local_74 = 0.0;
      for (local_40 = param_1; *local_40 != 0; local_40 = local_40 + 1) {
        if (*local_40 < 0x20) {
          FUN_00470ed0((void *)((int)this + 0x10),0x349 - (uint)*local_40);
        }
        else {
          FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x2a8);
        }
        iVar3 = FUN_00437cd0((int)this + 0x10);
        local_74 = *(float *)(iVar3 + 0x4c) / (DAT_0056c8d8 - DAT_005b8818) + local_74;
      }
      local_14 = (float)((uint)local_74 ^ _DAT_0056d7d0) * *(float *)(param_1 + 0x114);
      break;
    default:
      local_14 = (float)((uint)(float)(int)sVar2 ^ _DAT_0056d7d0) * local_44 * DAT_005b8818;
    }
  }
  if (*(int *)(param_1 + 0x13c) == 0) {
    local_10 = ((float)((uint)local_48 ^ _DAT_0056d7d0) / DAT_0056c8d0) * DAT_005b8818;
  }
  else if (*(int *)(param_1 + 0x13c) == 2) {
    local_10 = (float)((uint)local_48 ^ _DAT_0056d7d0) * DAT_005b8818;
  }
  FUN_00422e10(&local_38);
  FUN_00422e10(&local_20);
  FUN_00422e10(&local_2c);
  local_38 = local_14;
  local_34 = local_10;
  local_30 = local_c;
  for (local_40 = param_1; *local_40 != 0; local_40 = local_40 + 1) {
    local_2c = 0.0;
    local_28 = 0.0;
    if (*local_40 == 10) {
      local_14 = local_38;
      local_10 = local_48 * *(float *)(param_1 + 0x118) * DAT_005b8818 + local_10;
    }
    else if ((((*local_40 == 0x20) && (*(int *)(param_1 + 0x128) != 6)) &&
             (*(int *)(param_1 + 0x128) != 7)) &&
            ((*(int *)(param_1 + 0x128) != 8 && (*(int *)(param_1 + 0x128) != 9)))) {
      local_14 = local_44 * DAT_005b8818 + local_14;
    }
    else {
      switch(*(undefined4 *)(param_1 + 0x128)) {
      case 0:
        FUN_00470ed0((void *)((int)this + 0x10),*local_40 - 0x20);
        break;
      case 1:
        FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x42);
        break;
      case 2:
      case 3:
        local_44 = DAT_0056f7a0 * *(float *)(param_1 + 0x114);
        if ((*local_40 < 0x61) || (0x7a < *local_40)) {
          if ((*local_40 < 0x41) || (0x5a < *local_40)) {
            if (*local_40 == 0x2f) {
              FUN_00470ed0((void *)((int)this + 0x10),0xce);
            }
            else if (*local_40 == 0x3a) {
              FUN_00470ed0((void *)((int)this + 0x10),0xcf);
            }
            else if (*local_40 == 0x2d) {
              FUN_00470ed0((void *)((int)this + 0x10),0xd0);
            }
            else if (*local_40 == 0x2a) {
              FUN_00470ed0((void *)((int)this + 0x10),0xd1);
            }
            else if (*local_40 == 0x25) {
              FUN_00470ed0((void *)((int)this + 0x10),0xd2);
            }
            else if (*local_40 == 0x24) {
              FUN_00470ed0((void *)((int)this + 0x10),0x11f);
            }
            else if (*local_40 == 0x2e) {
              FUN_00470ed0((void *)((int)this + 0x10),0xd3);
              local_44 = DAT_0056c8e0 * *(float *)(param_1 + 0x114);
            }
            else if (*local_40 == 0x2b) {
              FUN_00470ed0((void *)((int)this + 0x10),0xd4);
            }
            else {
              FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x94);
            }
          }
          else {
            FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x94);
          }
        }
        else {
          FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x74);
        }
        break;
      case 4:
      case 5:
      case 10:
      case 0xb:
        if (*(int *)(param_1 + 0x128) == 10) {
          local_98 = 0x10d;
        }
        else {
          if (*(int *)(param_1 + 0x128) == 0xb) {
            local_94 = 0xfe;
          }
          else {
            local_94 = 0xef;
          }
          local_98 = local_94;
        }
        local_44 = DAT_0056f7a8 * *(float *)(param_1 + 0x114);
        if (*local_40 == 0x2f) {
          FUN_00470ed0((void *)((int)this + 0x10),local_98 + 10);
        }
        else if (*local_40 == 0x2e) {
          FUN_00470ed0((void *)((int)this + 0x10),local_98 + 0xb);
        }
        else if (*local_40 == 0x73) {
          FUN_00470ed0((void *)((int)this + 0x10),local_98 + 0xc);
        }
        else if (*local_40 == 0x2a) {
          FUN_00470ed0((void *)((int)this + 0x10),local_98 + 0xd);
        }
        else if (*local_40 == 0x2c) {
          FUN_00470ed0((void *)((int)this + 0x10),local_98 + 0xe);
          local_44 = DAT_0056c8e0 * *(float *)(param_1 + 0x114);
          local_28 = DAT_0056c8d8 * DAT_005b8818;
        }
        else {
          FUN_00470ed0((void *)((int)this + 0x10),(*local_40 - 0x30) + local_98);
        }
        break;
      case 6:
        FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x120);
        break;
      case 7:
        FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x1e4);
        break;
      case 8:
        FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x182);
        break;
      case 9:
        FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x246);
        break;
      case 0xc:
        if (*local_40 < 0x20) {
          FUN_00470ed0((void *)((int)this + 0x10),0x349 - (uint)*local_40);
        }
        else {
          FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x2a8);
        }
        break;
      case 0xd:
        if (*local_40 < 0x20) {
          FUN_00470ed0((void *)((int)this + 0x10),0x3ad - (uint)*local_40);
        }
        else {
          FUN_00470ed0((void *)((int)this + 0x10),*local_40 + 0x30c);
        }
      }
      if (((((*(int *)(param_1 + 0x128) == 6) || (*(int *)(param_1 + 0x128) == 7)) ||
           (*(int *)(param_1 + 0x128) == 8)) ||
          ((*(int *)(param_1 + 0x128) == 9 || (*(int *)(param_1 + 0x128) == 0xc)))) ||
         (*(int *)(param_1 + 0x128) == 0xd)) {
        iVar3 = FUN_00437cd0((int)this + 0x10);
        local_44 = ((*(float *)(iVar3 + 0x4c) / (DAT_0056c8d8 - DAT_005b8818)) / DAT_005b8818) *
                   *(float *)(param_1 + 0x114);
        iVar3 = FUN_00437cd0((int)this + 0x10);
        fVar5 = (*(float *)(iVar3 + 0x48) / DAT_0056c8d0) * DAT_005b8818;
        iVar3 = FUN_00437cd0((int)this + 0x10);
        FUN_00470e90((void *)((int)this + 0x10),
                     (*(float *)(iVar3 + 0x4c) / DAT_0056c8d0) * DAT_005b8818,fVar5);
      }
      else {
        iVar3 = FUN_00437cd0((int)this + 0x10);
        uVar1 = *(undefined4 *)(iVar3 + 0x48);
        iVar3 = FUN_00437cd0((int)this + 0x10);
        FUN_00470e90((void *)((int)this + 0x10),*(undefined4 *)(iVar3 + 0x4c),uVar1);
      }
      FUN_004296e0(&local_2c,&local_14);
      FUN_00458fa0(&local_20,&local_20,&local_2c,*(float *)(param_1 + 0x11c));
      FUN_004296e0(&local_20,(float *)(param_1 + 0x100));
      FUN_00450590((void *)((int)this + 0x10),*(undefined4 *)(param_1 + 0x11c));
      if (*(int *)(param_1 + 300) != 0) {
        FUN_0044fa10((void *)((int)this + 0x10),*(uint *)(param_1 + 0x10c) & 0xff000000);
        FUN_00470b60((void *)((int)this + 0x10),
                     (byte)((uint)*(undefined4 *)(param_1 + 0x10c) >> 0x19));
        FUN_00470b20((void *)((int)this + 0x10),DAT_0056c8d0 * DAT_005b8818 + local_20);
        FUN_00470b40((void *)((int)this + 0x10),DAT_0056c8d0 * DAT_005b8818 + local_1c);
        fVar4 = FUN_004379c0((int)this + 0x10);
        if ((float)fVar4 == DAT_0056c8cc) {
          *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3 | 4;
        }
        else {
          *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xf3;
        }
        if (*(float *)(param_1 + 0x11c) == _DAT_0056e0e8) {
          FUN_0043f550(DAT_005c0028,(void *)((int)this + 0x10));
        }
        else {
          FUN_0043f630(DAT_005c0028,(void *)((int)this + 0x10));
        }
      }
      FUN_00470b20((void *)((int)this + 0x10),local_20);
      FUN_00470b40((void *)((int)this + 0x10),local_1c);
      FUN_0044fa10((void *)((int)this + 0x10),*(undefined4 *)(param_1 + 0x10c));
      if (*(float *)(param_1 + 0x11c) == _DAT_0056e0e8) {
        FUN_0043f550(DAT_005c0028,(void *)((int)this + 0x10));
      }
      else {
        FUN_0043f630(DAT_005c0028,(void *)((int)this + 0x10));
      }
      local_14 = local_44 * DAT_005b8818 + local_14;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

