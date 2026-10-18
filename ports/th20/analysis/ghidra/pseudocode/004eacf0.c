/* Entry: 0x004eacf0; symbol: FUN_004eacf0; body bytes: 2367 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl FUN_004eacf0(undefined4 *param_1,undefined4 param_2,double param_3,int *param_4)

{
  bool bVar1;
  uint *puVar2;
  undefined4 *puVar3;
  int *unaff_FS_OFFSET;
  float10 fVar4;
  undefined8 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int iVar8;
  size_t sVar9;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_6b0 [24];
  undefined1 local_698 [8];
  double local_690;
  locale local_688 [8];
  undefined4 local_680;
  undefined4 local_67c;
  void *local_678;
  locale *local_674;
  undefined8 local_670;
  undefined4 local_668;
  undefined4 local_664;
  undefined8 local_660;
  undefined8 local_658;
  locale *local_650;
  undefined4 local_64c;
  int local_648;
  uint local_644;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_640;
  int local_63c;
  int local_638;
  int local_634;
  int local_630;
  uint *local_62c;
  int local_628;
  uint *local_624;
  char local_61f;
  char local_61e;
  char local_61d;
  size_t local_61c;
  char local_616;
  char local_615;
  uint *local_614;
  uint *local_610;
  uint *local_60c;
  uint *local_608;
  uint *local_604;
  undefined4 local_600;
  undefined1 local_5fc [60];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_5c0 [24];
  int local_5a8;
  int local_5a4;
  int local_5a0;
  char local_59c;
  char local_59b;
  char local_59a;
  char local_599;
  undefined1 local_598 [1396];
  uint local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  int local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0056a1eb;
  local_1c = *unaff_FS_OFFSET;
  local_24 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_599 = *(char *)((int)param_4 + 10);
  if (local_599 == '\0') {
    local_599 = '\x02';
  }
  local_616 = '\0';
  local_628 = 3;
  local_615 = 'e';
  local_61c = param_4[1];
  local_61f = '\0';
  local_634 = (char)param_4[2] + -0x41;
  switch((char)param_4[2]) {
  case 'A':
    local_616 = '\x01';
  case 'a':
    local_628 = 4;
    local_615 = 'p';
    break;
  default:
    local_61f = '\x01';
    break;
  case 'E':
    local_616 = '\x01';
  case 'e':
    if (local_61c == 0xffffffff) {
      local_61c = 6;
    }
    local_628 = 1;
    break;
  case 'F':
    local_616 = '\x01';
  case 'f':
    if (local_61c == 0xffffffff) {
      local_61c = 6;
    }
    local_628 = 2;
    break;
  case 'G':
    local_616 = '\x01';
  case 'g':
    if (local_61c == 0xffffffff) {
      local_61c = 6;
    }
    else if (local_61c == 0) {
      local_61c = 1;
    }
    local_628 = 3;
  }
  local_67c = 0x432;
  local_680 = 0x569;
  local_630 = 0;
  if (0x432 < (int)local_61c) {
    local_630 = local_61c - 0x432;
    local_61c = 0x432;
  }
  local_59b = FUN_004fffa0(param_3);
  bVar1 = FUN_004f3790();
  if (bVar1) {
    local_604 = (uint *)local_598;
    if (local_59b != '\0') {
      local_604 = (uint *)(local_598 + 1);
    }
    FUN_00543e20(local_604,(uint *)&DAT_0056f58c,3);
    local_604 = (uint *)((int)local_604 + 3);
  }
  else {
    uVar6 = SUB84(param_3,0);
    uVar7 = (undefined4)((ulonglong)param_3 >> 0x20);
    if (local_61c == 0xffffffff) {
      if (local_61f == '\0') {
        iVar8 = local_628;
        puVar2 = (uint *)FUN_004f3340((int)local_598);
        uVar5 = FUN_00500170((uint *)local_598,puVar2,(double)CONCAT44(uVar7,uVar6),iVar8);
        local_658._0_4_ = (uint *)uVar5;
        local_658._4_4_ = (undefined4)((ulonglong)uVar5 >> 0x20);
        local_604 = (uint *)local_658;
        local_600 = local_658._4_4_;
        local_658 = uVar5;
      }
      else {
        puVar2 = (uint *)FUN_004f3340((int)local_598);
        uVar5 = FUN_00500130((uint *)local_598,puVar2,(double)CONCAT44(uVar7,uVar6));
        local_670._0_4_ = (uint *)uVar5;
        local_670._4_4_ = (undefined4)((ulonglong)uVar5 >> 0x20);
        local_604 = (uint *)local_670;
        local_600 = local_670._4_4_;
        local_670 = uVar5;
      }
    }
    else {
      iVar8 = local_628;
      sVar9 = local_61c;
      puVar2 = (uint *)FUN_004f3340((int)local_598);
      uVar5 = FUN_005001b0((uint *)local_598,puVar2,(double)CONCAT44(uVar7,uVar6),iVar8,sVar9);
      local_660._0_4_ = (uint *)uVar5;
      local_660._4_4_ = (undefined4)((ulonglong)uVar5 >> 0x20);
      local_604 = (uint *)local_660;
      local_600 = local_660._4_4_;
      local_660 = uVar5;
    }
  }
  local_608 = (uint *)local_598;
  local_5a0 = (int)local_604 - (int)local_608;
  if (local_59b == '\0') {
    if (local_599 != '\x02') {
      local_5a0 = local_5a0 + 1;
    }
  }
  else {
    local_608 = (uint *)(local_598 + 1);
  }
  if (local_616 != '\0') {
    FUN_004fb8c0((char *)local_608,(char *)local_604);
    local_615 = local_615 + -0x20;
  }
  local_61d = FUN_004f3710();
  local_59a = '\0';
  local_60c = local_604;
  local_610 = local_604;
  local_614 = local_604;
  local_5a4 = 0;
  local_5a8 = 0;
  FUN_0040dc60(local_5c0);
  local_14 = 0;
  if (local_61d != '\0') {
    if ((*(char *)((int)param_4 + 0xb) != '\0') || ((char)param_4[3] != '\0')) {
      for (local_624 = local_608; local_624 < local_604; local_624 = (uint *)((int)local_624 + 1)) {
        if ((char)*local_624 == '.') {
          local_610 = local_624;
        }
        else if ((char)*local_624 == local_615) {
          local_60c = local_624;
        }
      }
      local_614 = _Min_value<>((uint *)&local_610,(uint *)&local_60c);
      local_614 = (uint *)*local_614;
      if ((*(char *)((int)param_4 + 0xb) != '\0') && (local_610 == local_604)) {
        local_5a0 = local_5a0 + 1;
        local_59a = '\x01';
      }
      if ((char)param_4[3] != '\0') {
        local_674 = FUN_004fbf20(&stack0x00000018,local_688);
        local_14._0_1_ = 1;
        local_650 = local_674;
        local_678 = (void *)FUN_004f3910(local_674);
        local_640 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     *)FUN_004ff7a0(local_678,local_6b0);
        FUN_00418eb0(local_5c0,local_640);
        FUN_0040e5a0(local_6b0);
        local_14 = (uint)local_14._1_3_ << 8;
        FUN_004f5600((int)local_688);
        puVar3 = (undefined4 *)FUN_004494f0(local_5c0,local_698);
        local_668 = *puVar3;
        local_664 = puVar3[1];
        local_644 = (int)local_614 - (int)local_608;
        local_5a8 = FUN_004fb9c0(local_644);
        local_5a0 = local_5a0 + local_5a8;
      }
    }
    local_638 = local_628 + -1;
    switch(local_638) {
    case 0:
    case 3:
      if (((local_630 != 0) && (*(char *)((int)param_4 + 0xb) == '\0')) &&
         ((char)param_4[3] == '\0')) {
        do {
          local_60c = (uint *)((int)local_60c + -1);
        } while (*(char *)local_60c != local_615);
      }
    case 1:
      local_5a4 = local_630;
      break;
    case 2:
      if ((*(char *)((int)param_4 + 0xb) != '\0') &&
         (((char)param_4[2] == 'g' || ((char)param_4[2] == 'G')))) {
        local_63c = (int)local_60c - (int)local_608;
        if (local_59a == '\0') {
          local_63c = local_63c + -1;
        }
        local_5a4 = (local_630 + local_61c) - local_63c;
        if (local_60c == local_604) {
          fVar4 = FUN_004fe870(param_3);
          local_690 = (double)fVar4;
          if ((local_690 < DAT_0056c468) && (param_3 != _DAT_0056cd50)) {
            for (local_62c = local_608; local_62c < local_604;
                local_62c = (uint *)((int)local_62c + 1)) {
              if ((char)*local_62c == '0') {
                local_5a4 = local_5a4 + 1;
              }
              else if ((char)*local_62c != '.') break;
            }
          }
        }
      }
    }
  }
  local_5a0 = local_5a0 + local_5a4;
  if (((*(char *)((int)param_4 + 0xd) == '\0') || (*(char *)((int)param_4 + 9) != '\0')) ||
     (local_61d == '\0')) {
    local_61e = '\0';
  }
  else {
    local_61e = '\x01';
  }
  local_59c = local_61e;
  FUN_004f3c60(local_5fc,&local_599,&local_59b,&local_59c,&local_5a0,param_4,&stack0x00000018,
               &local_608,&local_614,local_5c0,&local_5a8,&local_610,&local_604,&local_59a,
               &local_60c,&local_5a4);
  if (local_59c == '\0') {
    local_648 = local_5a0;
    puVar3 = (undefined4 *)move<>(&param_2);
    local_64c = *puVar3;
    FUN_004ef680(param_1,local_64c,local_648,param_4,'\x02',local_5fc);
    local_14 = 0xffffffff;
    FUN_0040e5a0(local_5c0);
  }
  else {
    puVar3 = (undefined4 *)move<>(&param_2);
    FUN_004f5b90(local_5fc,param_1,*puVar3);
    local_14 = 0xffffffff;
    FUN_0040e5a0(local_5c0);
  }
  *unaff_FS_OFFSET = local_1c;
  FUN_005429ee(local_24 ^ (uint)&stack0xfffffff0);
  return;
}

