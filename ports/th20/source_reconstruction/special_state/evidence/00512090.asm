00512090 55                       push       ebp
00512091 8bec                     mov        ebp, esp
00512093 51                       push       ecx
00512094 894dfc                   mov        dword ptr [ebp - 4], ecx
00512097 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051209a e891f9ffff               call       0x511a30
0051209f 8b4508                   mov        eax, dword ptr [ebp + 8]
005120a2 83e001                   and        eax, 1
005120a5 7411                     je         0x5120b8
005120a7 68b4000000               push       0xb4
005120ac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005120af 51                       push       ecx
005120b0 e8d8060300               call       0x54278d
005120b5 83c408                   add        esp, 8
005120b8 8b45fc                   mov        eax, dword ptr [ebp - 4]
005120bb 8be5                     mov        esp, ebp
005120bd 5d                       pop        ebp
005120be c20400                   ret        4
005120c1 cc                       int3       
005120c2 cc                       int3       
005120c3 cc                       int3       
005120c4 cc                       int3       
005120c5 cc                       int3       
005120c6 cc                       int3       
005120c7 cc                       int3       
005120c8 cc                       int3       
005120c9 cc                       int3       
005120ca cc                       int3       
005120cb cc                       int3       
005120cc cc                       int3       
005120cd cc                       int3       
005120ce cc                       int3       
005120cf cc                       int3       
