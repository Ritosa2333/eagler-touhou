00447c10 55                       push       ebp
00447c11 8bec                     mov        ebp, esp
00447c13 6aff                     push       -1
00447c15 6800765600               push       0x567600
00447c1a 64a100000000             mov        eax, dword ptr fs:[0]
00447c20 50                       push       eax
00447c21 51                       push       ecx
00447c22 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00447c27 33c5                     xor        eax, ebp
00447c29 50                       push       eax
00447c2a 8d45f4                   lea        eax, [ebp - 0xc]
00447c2d 64a300000000             mov        dword ptr fs:[0], eax
00447c33 894df0                   mov        dword ptr [ebp - 0x10], ecx
00447c36 68408d4400               push       0x448d40
00447c3b 6a01                     push       1
00447c3d 6a48                     push       0x48
00447c3f 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00447c42 50                       push       eax
00447c43 e8d83ffcff               call       0x40bc20
00447c48 90                       nop        
00447c49 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00447c4c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00447c4f 64890d00000000           mov        dword ptr fs:[0], ecx
00447c56 59                       pop        ecx
00447c57 8be5                     mov        esp, ebp
00447c59 5d                       pop        ebp
00447c5a c3                       ret        
00447c5b cc                       int3       
00447c5c cc                       int3       
00447c5d cc                       int3       
00447c5e cc                       int3       
00447c5f cc                       int3       
