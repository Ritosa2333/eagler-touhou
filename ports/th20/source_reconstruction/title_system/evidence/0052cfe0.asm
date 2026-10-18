0052cfe0 55                       push       ebp
0052cfe1 8bec                     mov        ebp, esp
0052cfe3 83ec0c                   sub        esp, 0xc
0052cfe6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052cfeb 33c5                     xor        eax, ebp
0052cfed 8945fc                   mov        dword ptr [ebp - 4], eax
0052cff0 6818505700               push       0x575018
0052cff5 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0052cffb e80009ffff               call       0x51d900
0052d000 8945f8                   mov        dword ptr [ebp - 8], eax
0052d003 c70558885b0000000000     mov        dword ptr [0x5b8858], 0
0052d00d 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052d010 0568590000               add        eax, 0x5968
0052d015 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052d018 8d4df8                   lea        ecx, [ebp - 8]
0052d01b 51                       push       ecx
0052d01c 68e0f75100               push       0x51f7e0
0052d021 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052d024 e80702ffff               call       0x51d230
0052d029 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052d02c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052d02f 33cd                     xor        ecx, ebp
0052d031 e8b8590100               call       0x5429ee
0052d036 8be5                     mov        esp, ebp
0052d038 5d                       pop        ebp
0052d039 c3                       ret        
0052d03a cc                       int3       
0052d03b cc                       int3       
0052d03c cc                       int3       
0052d03d cc                       int3       
0052d03e cc                       int3       
0052d03f cc                       int3       
