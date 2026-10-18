00478190 55                       push       ebp
00478191 8bec                     mov        ebp, esp
00478193 6aff                     push       -1
00478195 688d835600               push       0x56838d
0047819a 64a100000000             mov        eax, dword ptr fs:[0]
004781a0 50                       push       eax
004781a1 83ec20                   sub        esp, 0x20
004781a4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004781a9 33c5                     xor        eax, ebp
004781ab 8945f0                   mov        dword ptr [ebp - 0x10], eax
004781ae 50                       push       eax
004781af 8d45f4                   lea        eax, [ebp - 0xc]
004781b2 64a300000000             mov        dword ptr fs:[0], eax
004781b8 894de0                   mov        dword ptr [ebp - 0x20], ecx
004781bb 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004781be 0508010000               add        eax, 0x108
004781c3 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004781c6 6a08                     push       8
004781c8 8d4de8                   lea        ecx, [ebp - 0x18]
004781cb e8b03ef9ff               call       0x40c080
004781d0 8d4de8                   lea        ecx, [ebp - 0x18]
004781d3 51                       push       ecx
004781d4 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004781d7 e8a4a0f9ff               call       0x412280
004781dc c745fc00000000           mov        dword ptr [ebp - 4], 0
004781e3 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004781e6 e855a3f9ff               call       0x412540
004781eb 8945dc                   mov        dword ptr [ebp - 0x24], eax
004781ee eb09                     jmp        0x4781f9
004781f0 8d4de8                   lea        ecx, [ebp - 0x18]
004781f3 e8389af9ff               call       0x411c30
004781f8 90                       nop        
004781f9 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004781fc 52                       push       edx
004781fd 8d4de8                   lea        ecx, [ebp - 0x18]
00478200 e8ab99f9ff               call       0x411bb0
00478205 0fb6c0                   movzx      eax, al
00478208 85c0                     test       eax, eax
0047820a 7423                     je         0x47822f
0047820c 8d4de8                   lea        ecx, [ebp - 0x18]
0047820f e8ec40f9ff               call       0x40c300
00478214 8945d8                   mov        dword ptr [ebp - 0x28], eax
00478217 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0047821a e8e140f9ff               call       0x40c300
0047821f 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00478222 6a01                     push       1
00478224 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00478227 e834000000               call       0x478260
0047822c 90                       nop        
0047822d ebc1                     jmp        0x4781f0
0047822f c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00478236 8d4de8                   lea        ecx, [ebp - 0x18]
00478239 e8c298f9ff               call       0x411b00
0047823e 90                       nop        
0047823f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00478242 64890d00000000           mov        dword ptr fs:[0], ecx
00478249 59                       pop        ecx
0047824a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047824d 33cd                     xor        ecx, ebp
0047824f e89aa70c00               call       0x5429ee
00478254 8be5                     mov        esp, ebp
00478256 5d                       pop        ebp
00478257 c3                       ret        
00478258 cc                       int3       
00478259 cc                       int3       
0047825a cc                       int3       
0047825b cc                       int3       
0047825c cc                       int3       
0047825d cc                       int3       
0047825e cc                       int3       
0047825f cc                       int3       
