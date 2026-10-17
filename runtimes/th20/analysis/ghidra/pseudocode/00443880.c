/* Entry: 0x00443880; symbol: FUN_00443880; body bytes: 5993 */

void __thiscall FUN_00443880(void *this,void *param_1)

{
  int iVar1;
  int iVar2;
  byte bVar3;
  undefined3 extraout_var;
  void *pvVar4;
  float fVar5;
  float fVar6;
  float10 fVar7;
  float fVar8;
  float local_34;
  float local_30;
  float local_24;
  float local_20;
  float local_1c;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((*(uint *)((int)param_1 + 0x49c) & 1) != 0) {
    if (*(int *)((int)param_1 + 0x568) != 0) {
      (**(code **)(**(int **)((int)param_1 + 0x568) + 8))();
    }
    bVar3 = FUN_00445ab0((int)param_1);
    if ((CONCAT31(extraout_var,bVar3) != 0) && (*(int *)((int)param_1 + 0x570) == 0)) {
      FUN_004ddf80(0x5c4d40);
      switch(*(undefined1 *)((int)param_1 + 0x498)) {
      case 0:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_0043f550(this,param_1);
        }
        break;
      case 1:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_0043f630(this,param_1);
        }
        break;
      case 2:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_0043f5c0(this,param_1);
        }
        break;
      case 3:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_0043f630(this,param_1);
        }
        break;
      case 4:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_00440310(this,(int)param_1);
        }
        break;
      case 5:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_004413a0(this,(int)param_1);
        }
        break;
      case 6:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_004408b0((int)param_1);
        }
        break;
      case 7:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_00441c00(this,(int)param_1);
        }
        break;
      case 8:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_00441f00(this,param_1);
        }
        break;
      case 9:
      case 0xc:
      case 0xd:
      case 0xe:
        FUN_00445350(this,(int)param_1,*(float **)((int)param_1 + 0x564),
                     *(int *)((int)param_1 + 0x444) << 1);
        break;
      case 0xb:
        FUN_00445130(this,(int)param_1,*(float **)((int)param_1 + 0x564),
                     *(int *)((int)param_1 + 0x444) << 1);
        break;
      case 0xf:
        if ((*(char *)((int)param_1 + 0x493) != '\0') || (*(char *)((int)param_1 + 0x497) != '\0'))
        {
          FUN_004ddac0(0x5c4d40);
          FUN_00441f00(this,param_1);
          FUN_004dda60(0x5c4d40);
        }
        break;
      case 0x10:
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
      case 0x16:
      case 0x1a:
      case 0x1b:
      case 0x1c:
      case 0x1d:
      case 0x1e:
      case 0x1f:
      case 0x20:
      case 0x21:
      case 0x22:
      case 0x23:
      case 0x24:
      case 0x25:
      case 0x26:
      case 0x27:
      case 0x28:
      case 0x29:
      case 0x2a:
      case 0x2b:
      case 0x2c:
      case 0x2d:
      case 0x2e:
        FUN_00422e10(&local_14);
        fVar7 = FUN_00437930((int)param_1);
        local_24 = (float)fVar7;
        fVar7 = FUN_004459d0((int)param_1);
        local_20 = (float)fVar7;
        fVar7 = FUN_004459a0((int)param_1);
        local_1c = (float)fVar7;
        FUN_004376e0(param_1,&local_14);
        if ((*(int *)((int)param_1 + 0x55c) != 0) &&
           ((*(uint *)((int)param_1 + 0x49c) >> 0xc & 1) == 0)) {
          fVar7 = FUN_00437930(*(int *)((int)param_1 + 0x55c));
          local_24 = (float)fVar7 + local_24;
        }
        FUN_00445bc0(this,(int)param_1);
        if ((*(char *)((int)param_1 + 0x4a4) == '\x01') ||
           (*(char *)((int)param_1 + 0x4a4) == '\x05')) {
          local_20 = local_20 * DAT_005b8818;
          local_1c = local_1c * DAT_005b8818;
        }
        else if ((*(char *)((int)param_1 + 0x4a4) == '\x02') ||
                (*(char *)((int)param_1 + 0x4a4) == '\x06')) {
          local_20 = DAT_005b8818 * DAT_0056e0ec * local_20;
          local_1c = DAT_005b8818 * DAT_0056e0ec * local_1c;
        }
        switch(*(undefined1 *)((int)param_1 + 0x498)) {
        case 0x10:
          iVar2 = *(int *)((int)param_1 + 0x4ac);
          iVar1 = *(int *)((int)param_1 + 0x4a8);
          fVar5 = (float)FUN_004456e0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_00439a00(this,local_14,local_10,local_20,local_1c,local_24,fVar6,fVar5,iVar1,iVar2);
          break;
        case 0x11:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043b960(this,local_14,local_10,local_20,local_24,*(int *)((int)param_1 + 0x444),fVar6
                       ,fVar5);
          break;
        case 0x12:
          fVar5 = (float)FUN_004456e0((int)param_1);
          FUN_0043bc60(this,local_14,local_10,local_20,local_24,*(int *)((int)param_1 + 0x444),fVar5
                      );
          break;
        case 0x13:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043be80(this,local_14,local_10,local_20,local_1c,local_24,
                       *(int *)((int)param_1 + 0x444),fVar6,fVar5);
          break;
        case 0x14:
          iVar2 = *(int *)((int)param_1 + 0x4ac);
          iVar1 = *(int *)((int)param_1 + 0x4a8);
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_00439a00(this,local_14,local_10,local_20,local_1c,local_24,fVar6,fVar5,iVar1,iVar2);
          break;
        case 0x15:
          iVar2 = *(int *)((int)param_1 + 0x4ac);
          iVar1 = *(int *)((int)param_1 + 0x4a8);
          fVar5 = (float)FUN_004456e0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043a1c0(this,local_14,local_10,local_20,local_1c,local_24,fVar6,fVar5,iVar1,iVar2);
          break;
        case 0x16:
          iVar2 = *(int *)((int)param_1 + 0x4ac);
          iVar1 = *(int *)((int)param_1 + 0x4a8);
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043a1c0(this,local_14,local_10,local_20,local_1c,local_24,fVar6,fVar5,iVar1,iVar2);
          break;
        case 0x1a:
          iVar2 = *(int *)((int)param_1 + 0x4a8);
          if ((*(byte *)((int)param_1 + 0x4a1) >> 2 & 7) == 0) {
            local_30 = (float)FUN_004456e0((int)param_1);
          }
          else {
            local_30 = (float)FUN_004456c0((int)param_1);
          }
          fVar5 = (float)FUN_004456e0((int)param_1);
          FUN_0043cd40(this,local_14,local_10,local_20,local_24,fVar5,local_30,iVar2);
          break;
        case 0x1b:
          iVar2 = *(int *)((int)param_1 + 0x4ac);
          iVar1 = *(int *)((int)param_1 + 0x4a8);
          if ((*(byte *)((int)param_1 + 0x4a1) >> 2 & 7) == 0) {
            local_34 = (float)FUN_004456e0((int)param_1);
          }
          else {
            local_34 = (float)FUN_004456c0((int)param_1);
          }
          fVar5 = (float)FUN_004456e0((int)param_1);
          FUN_0043b0e0(this,local_14,local_10,local_20,local_1c,local_24,fVar5,local_34,iVar1,iVar2)
          ;
          break;
        case 0x1c:
          iVar2 = *(int *)((int)param_1 + 0x4ac);
          iVar1 = *(int *)((int)param_1 + 0x4a8);
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043e2c0(this,local_14,local_10,local_20,local_1c,local_24,fVar6,fVar5,iVar1,iVar2);
          break;
        case 0x1d:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043be80(this,local_14,local_10,local_1c / DAT_0056c8d0 + local_20,local_1c,local_24,
                       *(int *)((int)param_1 + 0x444),fVar6,fVar5);
          break;
        case 0x1e:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043be80(this,local_14,local_10,local_20 - local_1c / DAT_0056c8d0,local_1c,local_24,
                       *(int *)((int)param_1 + 0x444),fVar6,fVar5);
          break;
        case 0x1f:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043c180(this,local_14,local_10,(float)fVar7,local_20,local_24,iVar2,fVar6,fVar5);
          break;
        case 0x20:
          fVar5 = (float)FUN_004456e0((int)param_1);
          FUN_0043d830(this,local_14,local_10,fVar5);
          break;
        case 0x21:
          FUN_004456c0((int)param_1);
          fVar5 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043c4e0(this,local_14,local_10,(float)fVar7,local_20,local_24,iVar2,fVar5);
          break;
        case 0x22:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043c780(this,local_14,local_10,(float)fVar7,local_20,local_1c,local_24,iVar2,fVar6,
                       fVar5);
          break;
        case 0x23:
          FUN_00445880((int)param_1);
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar8 = local_1c / DAT_0056c8d0;
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043c780(this,local_14,local_10,(float)fVar7 + fVar8,
                       local_1c / DAT_0056c8d0 + local_20,local_1c,local_24,iVar2,fVar6,fVar5);
          break;
        case 0x24:
          FUN_00445880((int)param_1);
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar8 = local_1c / DAT_0056c8d0;
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043c780(this,local_14,local_10,(float)fVar7 - fVar8,
                       local_20 - local_1c / DAT_0056c8d0,local_1c,local_24,iVar2,fVar6,fVar5);
          break;
        case 0x25:
          pvVar4 = (void *)FUN_004456c0((int)param_1);
          fVar5 = (float)FUN_004456e0((int)param_1);
          FUN_0043a2b0(this,local_14,local_10,local_20,local_1c,*(float *)((int)param_1 + 0x454),
                       local_24,*(void **)((int)param_1 + 0x444),fVar5,pvVar4);
          break;
        case 0x26:
          fVar5 = (float)FUN_004456e0((int)param_1);
          FUN_0043a750(this,local_14,local_10,local_20,local_1c,*(float *)((int)param_1 + 0x454),
                       local_24,*(int *)((int)param_1 + 0x444),fVar5);
          break;
        case 0x27:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043ab20(this,local_14,local_10,
                       local_20 - *(float *)((int)param_1 + 0x454) * DAT_0056c8d0,
                       local_1c - *(float *)((int)param_1 + 0x454) * DAT_0056c8d0,
                       *(float *)((int)param_1 + 0x454),*(float *)((int)param_1 + 0x458),local_24,
                       *(int *)((int)param_1 + 0x444),fVar6,fVar5);
          break;
        case 0x28:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043ab20(this,local_14,local_10,
                       local_20 - *(float *)((int)param_1 + 0x454) * DAT_0056c8d0,
                       local_1c - *(float *)((int)param_1 + 0x454) * DAT_0056c8d0,
                       *(float *)((int)param_1 + 0x458) / DAT_0056c8d0 +
                       *(float *)((int)param_1 + 0x454),*(float *)((int)param_1 + 0x458),local_24,
                       *(int *)((int)param_1 + 0x444),fVar6,fVar5);
          break;
        case 0x29:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          FUN_0043ab20(this,local_14,local_10,
                       local_20 - *(float *)((int)param_1 + 0x454) * DAT_0056c8d0,
                       local_1c - *(float *)((int)param_1 + 0x454) * DAT_0056c8d0,
                       *(float *)((int)param_1 + 0x454) -
                       *(float *)((int)param_1 + 0x458) / DAT_0056c8d0,
                       *(float *)((int)param_1 + 0x458),local_24,*(int *)((int)param_1 + 0x444),
                       fVar6,fVar5);
          break;
        case 0x2a:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043d9b0(this,local_14,local_10,local_20,(float)fVar7,local_24,iVar2,fVar6,fVar5);
          break;
        case 0x2b:
          fVar5 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043dce0(this,local_14,local_10,local_20,(float)fVar7,local_24,iVar2,fVar5);
          break;
        case 0x2c:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043df30(this,local_14,local_10,local_20,(float)fVar7,local_1c,local_24,iVar2,fVar6,
                       fVar5);
          break;
        case 0x2d:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar8 = local_1c / DAT_0056c8d0;
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043df30(this,local_14,local_10,local_1c / DAT_0056c8d0 + local_20,
                       (float)fVar7 + fVar8,local_1c,local_24,iVar2,fVar6,fVar5);
          break;
        case 0x2e:
          fVar5 = (float)FUN_004456c0((int)param_1);
          fVar6 = (float)FUN_004456e0((int)param_1);
          iVar2 = *(int *)((int)param_1 + 0x444);
          fVar8 = local_1c / DAT_0056c8d0;
          fVar7 = FUN_00445880((int)param_1);
          FUN_0043df30(this,local_14,local_10,local_20 - local_1c / DAT_0056c8d0,
                       (float)fVar7 - fVar8,local_1c,local_24,iVar2,fVar6,fVar5);
        }
      default:
        break;
      case 0x18:
      case 0x19:
        FUN_00443020(this,(int)param_1,*(undefined4 *)((int)param_1 + 0x564),
                     *(int *)((int)param_1 + 0x444) << 1);
        break;
      case 0x2f:
        FUN_00443020(this,(int)param_1,*(undefined4 *)((int)param_1 + 0x564),
                     *(int *)((int)param_1 + 0x444) << 1);
        break;
      case 0x30:
        FUN_004ddac0(0x5c4d40);
        FUN_00443020(this,(int)param_1,*(undefined4 *)((int)param_1 + 0x564),3);
        FUN_004dda60(0x5c4d40);
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

