0051f490 55                       push       ebp
0051f491 8bec                     mov        ebp, esp
0051f493 6aff                     push       -1
0051f495 680baf5600               push       0x56af0b
0051f49a 64a100000000             mov        eax, dword ptr fs:[0]
0051f4a0 50                       push       eax
0051f4a1 b820330000               mov        eax, 0x3320
0051f4a6 e895370200               call       0x542c40
0051f4ab a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051f4b0 33c5                     xor        eax, ebp
0051f4b2 8945f0                   mov        dword ptr [ebp - 0x10], eax
0051f4b5 50                       push       eax
0051f4b6 8d45f4                   lea        eax, [ebp - 0xc]
0051f4b9 64a300000000             mov        dword ptr fs:[0], eax
0051f4bf 898d2ccdffff             mov        dword ptr [ebp - 0x32d4], ecx
0051f4c5 c78528cdffff01000000     mov        dword ptr [ebp - 0x32d8], 1
0051f4cf eb0f                     jmp        0x51f4e0
0051f4d1 8b8528cdffff             mov        eax, dword ptr [ebp - 0x32d8]
0051f4d7 83c001                   add        eax, 1
0051f4da 898528cdffff             mov        dword ptr [ebp - 0x32d8], eax
0051f4e0 83bd28cdffff19           cmp        dword ptr [ebp - 0x32d8], 0x19
0051f4e7 7f76                     jg         0x51f55f
0051f4e9 8b8d28cdffff             mov        ecx, dword ptr [ebp - 0x32d8]
0051f4ef 51                       push       ecx
0051f4f0 6834265700               push       0x572634
0051f4f5 6a40                     push       0x40
0051f4f7 8d55b0                   lea        edx, [ebp - 0x50]
0051f4fa 52                       push       edx
0051f4fb e820fdefff               call       0x41f220
0051f500 83c410                   add        esp, 0x10
0051f503 8d45b0                   lea        eax, [ebp - 0x50]
0051f506 50                       push       eax
0051f507 e8d4b3feff               call       0x50a8e0
0051f50c 83c404                   add        esp, 4
0051f50f 898514cdffff             mov        dword ptr [ebp - 0x32ec], eax
0051f515 8b8d2ccdffff             mov        ecx, dword ptr [ebp - 0x32d4]
0051f51b 81c140570000             add        ecx, 0x5740
0051f521 898d18cdffff             mov        dword ptr [ebp - 0x32e8], ecx
0051f527 8b9528cdffff             mov        edx, dword ptr [ebp - 0x32d8]
0051f52d 83ea01                   sub        edx, 1
0051f530 52                       push       edx
0051f531 8b8d18cdffff             mov        ecx, dword ptr [ebp - 0x32e8]
0051f537 e84450efff               call       0x414580
0051f53c 8b8d14cdffff             mov        ecx, dword ptr [ebp - 0x32ec]
0051f542 8908                     mov        dword ptr [eax], ecx
0051f544 8b952ccdffff             mov        edx, dword ptr [ebp - 0x32d4]
0051f54a 8b82d4580000             mov        eax, dword ptr [edx + 0x58d4]
0051f550 c1e802                   shr        eax, 2
0051f553 83e001                   and        eax, 1
0051f556 7402                     je         0x51f55a
0051f558 eb05                     jmp        0x51f55f
0051f55a e972ffffff               jmp        0x51f4d1
0051f55f 6a00                     push       0
0051f561 68e1675b00               push       0x5b67e1
0051f566 8d8d98cfffff             lea        ecx, [ebp - 0x3068]
0051f56c e85f87efff               call       0x417cd0
0051f571 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051f578 68a4ca5600               push       0x56caa4
0051f57d 8d8d98cfffff             lea        ecx, [ebp - 0x3068]
0051f583 e8b888efff               call       0x417e40
0051f588 6838515700               push       0x575138
0051f58d 8d8d98cfffff             lea        ecx, [ebp - 0x3068]
0051f593 e8a888efff               call       0x417e40
0051f598 8d8d80cfffff             lea        ecx, [ebp - 0x3080]
0051f59e 51                       push       ecx
0051f59f 8d8d98cfffff             lea        ecx, [ebp - 0x3068]
0051f5a5 e8e6f1efff               call       0x41e790
0051f5aa c645fc01                 mov        byte ptr [ebp - 4], 1
0051f5ae 8d8d80cfffff             lea        ecx, [ebp - 0x3080]
0051f5b4 e87709efff               call       0x40ff30
0051f5b9 898520cdffff             mov        dword ptr [ebp - 0x32e0], eax
0051f5bf 6800200000               push       0x2000
0051f5c4 8d95b0cfffff             lea        edx, [ebp - 0x3050]
0051f5ca 52                       push       edx
0051f5cb 6aff                     push       -1
0051f5cd 8b8520cdffff             mov        eax, dword ptr [ebp - 0x32e0]
0051f5d3 50                       push       eax
0051f5d4 6a00                     push       0
0051f5d6 68a4030000               push       0x3a4
0051f5db ff1524c15600             call       dword ptr [0x56c124]
0051f5e1 8d8d30cdffff             lea        ecx, [ebp - 0x32d0]
0051f5e7 51                       push       ecx
0051f5e8 8d95b0cfffff             lea        edx, [ebp - 0x3050]
0051f5ee 52                       push       edx
0051f5ef ff1560c05600             call       dword ptr [0x56c060]
0051f5f5 89851ccdffff             mov        dword ptr [ebp - 0x32e4], eax
0051f5fb 83bd1ccdffffff           cmp        dword ptr [ebp - 0x32e4], -1
0051f602 0f8453010000             je         0x51f75b
0051f608 c78524cdffff19000000     mov        dword ptr [ebp - 0x32dc], 0x19
0051f612 eb0f                     jmp        0x51f623
0051f614 8b8524cdffff             mov        eax, dword ptr [ebp - 0x32dc]
0051f61a 83c001                   add        eax, 1
0051f61d 898524cdffff             mov        dword ptr [ebp - 0x32dc], eax
0051f623 83bd24cdffff4b           cmp        dword ptr [ebp - 0x32dc], 0x4b
0051f62a 0f8d2b010000             jge        0x51f75b
0051f630 6a00                     push       0
0051f632 6a00                     push       0
0051f634 6800100000               push       0x1000
0051f639 8d8db0efffff             lea        ecx, [ebp - 0x1050]
0051f63f 51                       push       ecx
0051f640 6aff                     push       -1
0051f642 8d955ccdffff             lea        edx, [ebp - 0x32a4]
0051f648 52                       push       edx
0051f649 6a00                     push       0
0051f64b 68a4030000               push       0x3a4
0051f650 ff1508c15600             call       dword ptr [0x56c108]
0051f656 8d85b0efffff             lea        eax, [ebp - 0x1050]
0051f65c 50                       push       eax
0051f65d 8d8d98cfffff             lea        ecx, [ebp - 0x3068]
0051f663 e88887efff               call       0x417df0
0051f668 90                       nop        
0051f669 8d8decccffff             lea        ecx, [ebp - 0x3314]
0051f66f 51                       push       ecx
0051f670 8d8d98cfffff             lea        ecx, [ebp - 0x3068]
0051f676 e815cff2ff               call       0x44c590
0051f67b 898510cdffff             mov        dword ptr [ebp - 0x32f0], eax
0051f681 8b9510cdffff             mov        edx, dword ptr [ebp - 0x32f0]
0051f687 52                       push       edx
0051f688 8d8d98cfffff             lea        ecx, [ebp - 0x3068]
0051f68e e87d99efff               call       0x419010
0051f693 8d8decccffff             lea        ecx, [ebp - 0x3314]
0051f699 e822f0eeff               call       0x40e6c0
0051f69e 90                       nop        
0051f69f 8d85d4ccffff             lea        eax, [ebp - 0x332c]
0051f6a5 50                       push       eax
0051f6a6 8d8d98cfffff             lea        ecx, [ebp - 0x3068]
0051f6ac e8dff0efff               call       0x41e790
0051f6b1 89850ccdffff             mov        dword ptr [ebp - 0x32f4], eax
0051f6b7 8b8d0ccdffff             mov        ecx, dword ptr [ebp - 0x32f4]
0051f6bd 51                       push       ecx
0051f6be 8d8d80cfffff             lea        ecx, [ebp - 0x3080]
0051f6c4 e8e797efff               call       0x418eb0
0051f6c9 8d8dd4ccffff             lea        ecx, [ebp - 0x332c]
0051f6cf e8cceeeeff               call       0x40e5a0
0051f6d4 90                       nop        
0051f6d5 8d8d80cfffff             lea        ecx, [ebp - 0x3080]
0051f6db e85008efff               call       0x40ff30
0051f6e0 898520cdffff             mov        dword ptr [ebp - 0x32e0], eax
0051f6e6 8b9520cdffff             mov        edx, dword ptr [ebp - 0x32e0]
0051f6ec 52                       push       edx
0051f6ed e8eeb1feff               call       0x50a8e0
0051f6f2 83c404                   add        esp, 4
0051f6f5 898504cdffff             mov        dword ptr [ebp - 0x32fc], eax
0051f6fb 8b852ccdffff             mov        eax, dword ptr [ebp - 0x32d4]
0051f701 0540570000               add        eax, 0x5740
0051f706 898508cdffff             mov        dword ptr [ebp - 0x32f8], eax
0051f70c 8b8d24cdffff             mov        ecx, dword ptr [ebp - 0x32dc]
0051f712 51                       push       ecx
0051f713 8b8d08cdffff             mov        ecx, dword ptr [ebp - 0x32f8]
0051f719 e8624eefff               call       0x414580
0051f71e 8b9504cdffff             mov        edx, dword ptr [ebp - 0x32fc]
0051f724 8910                     mov        dword ptr [eax], edx
0051f726 8b852ccdffff             mov        eax, dword ptr [ebp - 0x32d4]
0051f72c 8b88d4580000             mov        ecx, dword ptr [eax + 0x58d4]
0051f732 c1e902                   shr        ecx, 2
0051f735 83e101                   and        ecx, 1
0051f738 7402                     je         0x51f73c
0051f73a eb1f                     jmp        0x51f75b
0051f73c 8d9530cdffff             lea        edx, [ebp - 0x32d0]
0051f742 52                       push       edx
0051f743 8b851ccdffff             mov        eax, dword ptr [ebp - 0x32e4]
0051f749 50                       push       eax
0051f74a ff1564c05600             call       dword ptr [0x56c064]
0051f750 85c0                     test       eax, eax
0051f752 7502                     jne        0x51f756
0051f754 eb05                     jmp        0x51f75b
0051f756 e9b9feffff               jmp        0x51f614
0051f75b 8b8d1ccdffff             mov        ecx, dword ptr [ebp - 0x32e4]
0051f761 51                       push       ecx
0051f762 ff155cc05600             call       dword ptr [0x56c05c]
0051f768 c645fc00                 mov        byte ptr [ebp - 4], 0
0051f76c 8d8d80cfffff             lea        ecx, [ebp - 0x3080]
0051f772 e829eeeeff               call       0x40e5a0
0051f777 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051f77e 8d8d98cfffff             lea        ecx, [ebp - 0x3068]
0051f784 e837efeeff               call       0x40e6c0
0051f789 90                       nop        
0051f78a 8b952ccdffff             mov        edx, dword ptr [ebp - 0x32d4]
0051f790 8b82d4580000             mov        eax, dword ptr [edx + 0x58d4]
0051f796 83c808                   or         eax, 8
0051f799 8b8d2ccdffff             mov        ecx, dword ptr [ebp - 0x32d4]
0051f79f 8981d4580000             mov        dword ptr [ecx + 0x58d4], eax
0051f7a5 8b952ccdffff             mov        edx, dword ptr [ebp - 0x32d4]
0051f7ab 8b82d4580000             mov        eax, dword ptr [edx + 0x58d4]
0051f7b1 83e0fb                   and        eax, 0xfffffffb
0051f7b4 8b8d2ccdffff             mov        ecx, dword ptr [ebp - 0x32d4]
0051f7ba 8981d4580000             mov        dword ptr [ecx + 0x58d4], eax
0051f7c0 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051f7c3 64890d00000000           mov        dword ptr fs:[0], ecx
0051f7ca 59                       pop        ecx
0051f7cb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051f7ce 33cd                     xor        ecx, ebp
0051f7d0 e819320200               call       0x5429ee
0051f7d5 8be5                     mov        esp, ebp
0051f7d7 5d                       pop        ebp
0051f7d8 c3                       ret        
0051f7d9 cc                       int3       
0051f7da cc                       int3       
0051f7db cc                       int3       
0051f7dc cc                       int3       
0051f7dd cc                       int3       
0051f7de cc                       int3       
0051f7df cc                       int3       
