00511770 55                       push       ebp
00511771 8bec                     mov        ebp, esp
00511773 83ec0c                   sub        esp, 0xc
00511776 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051177b 33c5                     xor        eax, ebp
0051177d 8945fc                   mov        dword ptr [ebp - 4], eax
00511780 894df4                   mov        dword ptr [ebp - 0xc], ecx
00511783 837d0800                 cmp        dword ptr [ebp + 8], 0
00511787 7502                     jne        0x51178b
00511789 eb36                     jmp        0x5117c1
0051178b 8b4508                   mov        eax, dword ptr [ebp + 8]
0051178e 50                       push       eax
0051178f e86c010000               call       0x511900
00511794 83c404                   add        esp, 4
00511797 6a01                     push       1
00511799 b940025c00               mov        ecx, 0x5c0240
0051179e e86dabefff               call       0x40c310
005117a3 50                       push       eax
005117a4 8d4df8                   lea        ecx, [ebp - 8]
005117a7 e8849fefff               call       0x40b730
005117ac 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005117af 51                       push       ecx
005117b0 e847120300               call       0x5429fc
005117b5 83c404                   add        esp, 4
005117b8 8d4df8                   lea        ecx, [ebp - 8]
005117bb e840a1efff               call       0x40b900
005117c0 90                       nop        
005117c1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005117c4 33cd                     xor        ecx, ebp
005117c6 e823120300               call       0x5429ee
005117cb 8be5                     mov        esp, ebp
005117cd 5d                       pop        ebp
005117ce c20400                   ret        4
005117d1 cc                       int3       
005117d2 cc                       int3       
005117d3 cc                       int3       
005117d4 cc                       int3       
005117d5 cc                       int3       
005117d6 cc                       int3       
005117d7 cc                       int3       
005117d8 cc                       int3       
005117d9 cc                       int3       
005117da cc                       int3       
005117db cc                       int3       
005117dc cc                       int3       
005117dd cc                       int3       
005117de cc                       int3       
005117df cc                       int3       
