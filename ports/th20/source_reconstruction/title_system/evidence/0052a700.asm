0052a700 55                       push       ebp
0052a701 8bec                     mov        ebp, esp
0052a703 83ec30                   sub        esp, 0x30
0052a706 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052a70b 33c5                     xor        eax, ebp
0052a70d 8945fc                   mov        dword ptr [ebp - 4], eax
0052a710 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0052a713 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052a716 8b4820                   mov        ecx, dword ptr [eax + 0x20]
0052a719 894dd8                   mov        dword ptr [ebp - 0x28], ecx
0052a71c 837dd802                 cmp        dword ptr [ebp - 0x28], 2
0052a720 7411                     je         0x52a733
0052a722 837dd803                 cmp        dword ptr [ebp - 0x28], 3
0052a726 740b                     je         0x52a733
0052a728 837dd804                 cmp        dword ptr [ebp - 0x28], 4
0052a72c 7405                     je         0x52a733
0052a72e e94f010000               jmp        0x52a882
0052a733 51                       push       ecx
0052a734 0f57c0                   xorps      xmm0, xmm0
0052a737 f30f110424               movss      dword ptr [esp], xmm0
0052a73c 51                       push       ecx
0052a73d f30f1005dc3f5700         movss      xmm0, dword ptr [0x573fdc]
0052a745 f30f110424               movss      dword ptr [esp], xmm0
0052a74a 51                       push       ecx
0052a74b f30f100570565700         movss      xmm0, dword ptr [0x575670]
0052a753 f30f110424               movss      dword ptr [esp], xmm0
0052a758 8d4df0                   lea        ecx, [ebp - 0x10]
0052a75b e87086efff               call       0x422dd0
0052a760 8d4de4                   lea        ecx, [ebp - 0x1c]
0052a763 e8a886efff               call       0x422e10
0052a768 6a07                     push       7
0052a76a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052a770 e88be3f5ff               call       0x488b00
0052a775 6a01                     push       1
0052a777 6a00                     push       0
0052a779 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052a77f e89cdef8ff               call       0x4b8620
0052a784 6a00                     push       0
0052a786 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052a78c e88fc2fbff               call       0x4e6a20
0052a791 90                       nop        
0052a792 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0052a799 eb09                     jmp        0x52a7a4
0052a79b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052a79e 83c201                   add        edx, 1
0052a7a1 8955e0                   mov        dword ptr [ebp - 0x20], edx
0052a7a4 837de00a                 cmp        dword ptr [ebp - 0x20], 0xa
0052a7a8 0f8d9a000000             jge        0x52a848
0052a7ae 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0052a7b1 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052a7b4 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052a7b7 894de8                   mov        dword ptr [ebp - 0x18], ecx
0052a7ba 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052a7bd 8955ec                   mov        dword ptr [ebp - 0x14], edx
0052a7c0 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052a7c3 81c104590000             add        ecx, 0x5904
0052a7c9 e8c257eeff               call       0x40ff90
0052a7ce 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0052a7d1 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052a7d4 81c114590000             add        ecx, 0x5914
0052a7da e8b157eeff               call       0x40ff90
0052a7df 8945d0                   mov        dword ptr [ebp - 0x30], eax
0052a7e2 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
0052a7e5 50                       push       eax
0052a7e6 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0052a7e9 51                       push       ecx
0052a7ea 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0052a7ed 83c224                   add        edx, 0x24
0052a7f0 52                       push       edx
0052a7f1 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052a7f4 50                       push       eax
0052a7f5 8d4df0                   lea        ecx, [ebp - 0x10]
0052a7f8 51                       push       ecx
0052a7f9 e8920df4ff               call       0x46b590
0052a7fe 83c414                   add        esp, 0x14
0052a801 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052a804 8b0495a0505700           mov        eax, dword ptr [edx*4 + 0x5750a0]
0052a80b 50                       push       eax
0052a80c 8d4df0                   lea        ecx, [ebp - 0x10]
0052a80f 51                       push       ecx
0052a810 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0052a816 52                       push       edx
0052a817 e87421f4ff               call       0x46c990
0052a81c 83c40c                   add        esp, 0xc
0052a81f 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0052a822 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052a825 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052a828 894df4                   mov        dword ptr [ebp - 0xc], ecx
0052a82b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0052a82e 8955f8                   mov        dword ptr [ebp - 8], edx
0052a831 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
0052a836 f30f5805b0f75600         addss      xmm0, dword ptr [0x56f7b0]
0052a83e f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
0052a843 e953ffffff               jmp        0x52a79b
0052a848 6a01                     push       1
0052a84a 6a01                     push       1
0052a84c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052a852 e8c9ddf8ff               call       0x4b8620
0052a857 6a00                     push       0
0052a859 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052a85f e89ce2f5ff               call       0x488b00
0052a864 6aff                     push       -1
0052a866 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052a86c e85f62f4ff               call       0x470ad0
0052a871 68000000ff               push       0xff000000
0052a876 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052a87c e82f62f4ff               call       0x470ab0
0052a881 90                       nop        
0052a882 33c0                     xor        eax, eax
0052a884 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a887 33cd                     xor        ecx, ebp
0052a889 e860810100               call       0x5429ee
0052a88e 8be5                     mov        esp, ebp
0052a890 5d                       pop        ebp
0052a891 c3                       ret        
0052a892 cc                       int3       
0052a893 cc                       int3       
0052a894 cc                       int3       
0052a895 cc                       int3       
0052a896 cc                       int3       
0052a897 cc                       int3       
0052a898 cc                       int3       
0052a899 cc                       int3       
0052a89a cc                       int3       
0052a89b cc                       int3       
0052a89c cc                       int3       
0052a89d cc                       int3       
0052a89e cc                       int3       
0052a89f cc                       int3       
