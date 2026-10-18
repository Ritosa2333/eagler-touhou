00424e00 55                       push       ebp
00424e01 8bec                     mov        ebp, esp
00424e03 83ec14                   sub        esp, 0x14
00424e06 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00424e0b 33c5                     xor        eax, ebp
00424e0d 8945fc                   mov        dword ptr [ebp - 4], eax
00424e10 0f57c0                   xorps      xmm0, xmm0
00424e13 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00424e18 0f57c0                   xorps      xmm0, xmm0
00424e1b f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00424e20 f30f2a05f8875b00         cvtsi2ss   xmm0, dword ptr [0x5b87f8]
00424e28 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00424e2d f30f2a05fc875b00         cvtsi2ss   xmm0, dword ptr [0x5b87fc]
00424e35 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00424e3a 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00424e40 e87b070200               call       0x4455c0
00424e45 6a02                     push       2
00424e47 b9404d5c00               mov        ecx, 0x5c4d40
00424e4c e88f8effff               call       0x41dce0
00424e51 8b4508                   mov        eax, dword ptr [ebp + 8]
00424e54 8b4818                   mov        ecx, dword ptr [eax + 0x18]
00424e57 c1e118                   shl        ecx, 0x18
00424e5a 8b5508                   mov        edx, dword ptr [ebp + 8]
00424e5d 0b4a20                   or         ecx, dword ptr [edx + 0x20]
00424e60 51                       push       ecx
00424e61 8d45ec                   lea        eax, [ebp - 0x14]
00424e64 50                       push       eax
00424e65 e8e6e7ffff               call       0x423650
00424e6a 83c408                   add        esp, 8
00424e6d b801000000               mov        eax, 1
00424e72 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00424e75 33cd                     xor        ecx, ebp
00424e77 e872db1100               call       0x5429ee
00424e7c 8be5                     mov        esp, ebp
00424e7e 5d                       pop        ebp
00424e7f c3                       ret        
