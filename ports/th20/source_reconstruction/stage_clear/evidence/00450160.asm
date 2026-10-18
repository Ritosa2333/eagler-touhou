00450160 55                       push       ebp
00450161 8bec                     mov        ebp, esp
00450163 6aff                     push       -1
00450165 683d865600               push       0x56863d
0045016a 64a100000000             mov        eax, dword ptr fs:[0]
00450170 50                       push       eax
00450171 83ec24                   sub        esp, 0x24
00450174 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00450179 33c5                     xor        eax, ebp
0045017b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0045017e 50                       push       eax
0045017f 8d45f4                   lea        eax, [ebp - 0xc]
00450182 64a300000000             mov        dword ptr fs:[0], eax
00450188 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0045018b 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0045018e 8b889c040000             mov        ecx, dword ptr [eax + 0x49c]
00450194 83e1fe                   and        ecx, 0xfffffffe
00450197 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0045019a 898a9c040000             mov        dword ptr [edx + 0x49c], ecx
004501a0 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004501a3 0514050000               add        eax, 0x514
004501a8 8945dc                   mov        dword ptr [ebp - 0x24], eax
004501ab 6a01                     push       1
004501ad 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004501b0 e8fb92ffff               call       0x4494b0
004501b5 8bc8                     mov        ecx, eax
004501b7 e8d4fdfbff               call       0x40ff90
004501bc 85c0                     test       eax, eax
004501be 0f8493000000             je         0x450257
004501c4 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004501c7 81c114050000             add        ecx, 0x514
004501cd 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004501d0 6a01                     push       1
004501d2 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004501d5 e8d692ffff               call       0x4494b0
004501da 8bc8                     mov        ecx, eax
004501dc e8affdfbff               call       0x40ff90
004501e1 8945e0                   mov        dword ptr [ebp - 0x20], eax
004501e4 6a08                     push       8
004501e6 8d4de8                   lea        ecx, [ebp - 0x18]
004501e9 e892befbff               call       0x40c080
004501ee 8d55e8                   lea        edx, [ebp - 0x18]
004501f1 52                       push       edx
004501f2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004501f5 e8f6b7ffff               call       0x44b9f0
004501fa c745fc00000000           mov        dword ptr [ebp - 4], 0
00450201 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00450204 e83723fcff               call       0x412540
00450209 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0045020c eb09                     jmp        0x450217
0045020e 8d4de8                   lea        ecx, [ebp - 0x18]
00450211 e81a1afcff               call       0x411c30
00450216 90                       nop        
00450217 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
0045021a 50                       push       eax
0045021b 8d4de8                   lea        ecx, [ebp - 0x18]
0045021e e88d19fcff               call       0x411bb0
00450223 0fb6c8                   movzx      ecx, al
00450226 85c9                     test       ecx, ecx
00450228 741d                     je         0x450247
0045022a 8d4de8                   lea        ecx, [ebp - 0x18]
0045022d e8cec0fbff               call       0x40c300
00450232 8945d0                   mov        dword ptr [ebp - 0x30], eax
00450235 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00450238 e823effbff               call       0x40f160
0045023d 8b08                     mov        ecx, dword ptr [eax]
0045023f e81cffffff               call       0x450160
00450244 90                       nop        
00450245 ebc7                     jmp        0x45020e
00450247 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0045024e 8d4de8                   lea        ecx, [ebp - 0x18]
00450251 e8aa18fcff               call       0x411b00
00450256 90                       nop        
00450257 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045025a 64890d00000000           mov        dword ptr fs:[0], ecx
00450261 59                       pop        ecx
00450262 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00450265 33cd                     xor        ecx, ebp
00450267 e882270f00               call       0x5429ee
0045026c 8be5                     mov        esp, ebp
0045026e 5d                       pop        ebp
0045026f c3                       ret        
