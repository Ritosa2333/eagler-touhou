004234a0 55                       push       ebp
004234a1 8bec                     mov        ebp, esp
004234a3 6aff                     push       -1
004234a5 6800765600               push       0x567600
004234aa 64a100000000             mov        eax, dword ptr fs:[0]
004234b0 50                       push       eax
004234b1 51                       push       ecx
004234b2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004234b7 33c5                     xor        eax, ebp
004234b9 50                       push       eax
004234ba 8d45f4                   lea        eax, [ebp - 0xc]
004234bd 64a300000000             mov        dword ptr fs:[0], eax
004234c3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004234c6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004234c9 c70044d75600             mov        dword ptr [eax], 0x56d744
004234cf 68a8d75600               push       0x56d7a8
004234d4 e8d791feff               call       0x40c6b0
004234d9 83c404                   add        esp, 4
004234dc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004234df 8b5108                   mov        edx, dword ptr [ecx + 8]
004234e2 52                       push       edx
004234e3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004234e9 e8c2effeff               call       0x4124b0
004234ee 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004234f1 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004234f4 51                       push       ecx
004234f5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004234fb e8b0effeff               call       0x4124b0
00423500 90                       nop        
00423501 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00423504 e8e7c8ffff               call       0x41fdf0
00423509 90                       nop        
0042350a 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0042350d 64890d00000000           mov        dword ptr fs:[0], ecx
00423514 59                       pop        ecx
00423515 8be5                     mov        esp, ebp
00423517 5d                       pop        ebp
00423518 c3                       ret        
00423519 cc                       int3       
0042351a cc                       int3       
0042351b cc                       int3       
0042351c cc                       int3       
0042351d cc                       int3       
0042351e cc                       int3       
0042351f cc                       int3       
