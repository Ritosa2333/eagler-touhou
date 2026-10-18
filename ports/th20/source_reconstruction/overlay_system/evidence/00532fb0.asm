00532fb0 55                       push       ebp
00532fb1 8bec                     mov        ebp, esp
00532fb3 83ec0c                   sub        esp, 0xc
00532fb6 894dfc                   mov        dword ptr [ebp - 4], ecx
00532fb9 6834625700               push       0x576234
00532fbe e8ed96edff               call       0x40c6b0
00532fc3 83c404                   add        esp, 4
00532fc6 8b4508                   mov        eax, dword ptr [ebp + 8]
00532fc9 50                       push       eax
00532fca 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532fcd e8fe150000               call       0x5345d0
00532fd2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532fd5 8b9180000000             mov        edx, dword ptr [ecx + 0x80]
00532fdb 8955f4                   mov        dword ptr [ebp - 0xc], edx
00532fde 8b45fc                   mov        eax, dword ptr [ebp - 4]
00532fe1 50                       push       eax
00532fe2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00532fe5 e8c6170000               call       0x5347b0
00532fea 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00532fed 51                       push       ecx
00532fee 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00532ff4 e8a746f4ff               call       0x4776a0
00532ff9 90                       nop        
00532ffa 6850625700               push       0x576250
00532fff 6a17                     push       0x17
00533001 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00533007 e844bef1ff               call       0x44ee50
0053300c 8945f8                   mov        dword ptr [ebp - 8], eax
0053300f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00533012 8b45f8                   mov        eax, dword ptr [ebp - 8]
00533015 894224                   mov        dword ptr [edx + 0x24], eax
00533018 837df800                 cmp        dword ptr [ebp - 8], 0
0053301c 7517                     jne        0x533035
0053301e 6810f65600               push       0x56f610
00533023 6878065c00               push       0x5c0678
00533028 e82311f2ff               call       0x454150
0053302d 83c408                   add        esp, 8
00533030 83c8ff                   or         eax, 0xffffffff
00533033 eb62                     jmp        0x533097
00533035 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00533038 51                       push       ecx
00533039 6870405300               push       0x534070
0053303e 6a1b                     push       0x1b
00533040 e87bf2edff               call       0x4122c0
00533045 83c40c                   add        esp, 0xc
00533048 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053304b 894208                   mov        dword ptr [edx + 8], eax
0053304e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00533051 50                       push       eax
00533052 68a0de4900               push       0x49dea0
00533057 6a37                     push       0x37
00533059 e802f3edff               call       0x412360
0053305e 83c40c                   add        esp, 0xc
00533061 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00533064 89410c                   mov        dword ptr [ecx + 0xc], eax
00533067 6a01                     push       1
00533069 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053306c e80f100000               call       0x534080
00533071 6a00                     push       0
00533073 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00533076 e805100000               call       0x534080
0053307b 6a00                     push       0
0053307d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00533080 83c110                   add        ecx, 0x10
00533083 e89804efff               call       0x423520
00533088 6a00                     push       0
0053308a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053308d 83c164                   add        ecx, 0x64
00533090 e88b04efff               call       0x423520
00533095 33c0                     xor        eax, eax
00533097 8be5                     mov        esp, ebp
00533099 5d                       pop        ebp
0053309a c20400                   ret        4
0053309d cc                       int3       
0053309e cc                       int3       
0053309f cc                       int3       
