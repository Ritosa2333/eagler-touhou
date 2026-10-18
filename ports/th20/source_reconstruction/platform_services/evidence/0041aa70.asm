0041aa70 55                       push       ebp
0041aa71 8bec                     mov        ebp, esp
0041aa73 6aff                     push       -1
0041aa75 68b57f5600               push       0x567fb5
0041aa7a 64a100000000             mov        eax, dword ptr fs:[0]
0041aa80 50                       push       eax
0041aa81 83ec40                   sub        esp, 0x40
0041aa84 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0041aa89 33c5                     xor        eax, ebp
0041aa8b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0041aa8e 50                       push       eax
0041aa8f 8d45f4                   lea        eax, [ebp - 0xc]
0041aa92 64a300000000             mov        dword ptr fs:[0], eax
0041aa98 894dbc                   mov        dword ptr [ebp - 0x44], ecx
0041aa9b 6a00                     push       0
0041aa9d 68e1675b00               push       0x5b67e1
0041aaa2 8d4dd8                   lea        ecx, [ebp - 0x28]
0041aaa5 e826d2ffff               call       0x417cd0
0041aaaa c745fc00000000           mov        dword ptr [ebp - 4], 0
0041aab1 68a8c95600               push       0x56c9a8
0041aab6 8d4dd8                   lea        ecx, [ebp - 0x28]
0041aab9 e882d3ffff               call       0x417e40
0041aabe 8d45c0                   lea        eax, [ebp - 0x40]
0041aac1 50                       push       eax
0041aac2 8d4dd8                   lea        ecx, [ebp - 0x28]
0041aac5 e8c63c0000               call       0x41e790
0041aaca c645fc01                 mov        byte ptr [ebp - 4], 1
0041aace 8d4dc0                   lea        ecx, [ebp - 0x40]
0041aad1 e85a54ffff               call       0x40ff30
0041aad6 8945b8                   mov        dword ptr [ebp - 0x48], eax
0041aad9 68b0000000               push       0xb0
0041aade 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0041aae1 51                       push       ecx
0041aae2 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
0041aae5 52                       push       edx
0041aae6 e88563ffff               call       0x410e70
0041aaeb 83c40c                   add        esp, 0xc
0041aaee 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0041aaf1 c645fc00                 mov        byte ptr [ebp - 4], 0
0041aaf5 8d4dc0                   lea        ecx, [ebp - 0x40]
0041aaf8 e8a33affff               call       0x40e5a0
0041aafd c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0041ab04 8d4dd8                   lea        ecx, [ebp - 0x28]
0041ab07 e8b43bffff               call       0x40e6c0
0041ab0c 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
0041ab0f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0041ab12 64890d00000000           mov        dword ptr fs:[0], ecx
0041ab19 59                       pop        ecx
0041ab1a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0041ab1d 33cd                     xor        ecx, ebp
0041ab1f e8ca7e1200               call       0x5429ee
0041ab24 8be5                     mov        esp, ebp
0041ab26 5d                       pop        ebp
0041ab27 c3                       ret        
0041ab28 cc                       int3       
0041ab29 cc                       int3       
0041ab2a cc                       int3       
0041ab2b cc                       int3       
0041ab2c cc                       int3       
0041ab2d cc                       int3       
0041ab2e cc                       int3       
0041ab2f cc                       int3       
