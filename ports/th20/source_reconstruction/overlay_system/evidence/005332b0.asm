005332b0 55                       push       ebp
005332b1 8bec                     mov        ebp, esp
005332b3 83ec24                   sub        esp, 0x24
005332b6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005332bb 33c5                     xor        eax, ebp
005332bd 8945fc                   mov        dword ptr [ebp - 4], eax
005332c0 894de8                   mov        dword ptr [ebp - 0x18], ecx
005332c3 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005332c6 e8b5e3eeff               call       0x421680
005332cb 6a00                     push       0
005332cd e8ae0df3ff               call       0x464080
005332d2 83c404                   add        esp, 4
005332d5 8bc8                     mov        ecx, eax
005332d7 e854f4edff               call       0x412730
005332dc 50                       push       eax
005332dd 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005332e0 e89b0d0000               call       0x534080
005332e5 90                       nop        
005332e6 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
005332ec e83f55f5ff               call       0x488830
005332f1 85c0                     test       eax, eax
005332f3 0f8493000000             je         0x53338c
005332f9 c745f400000000           mov        dword ptr [ebp - 0xc], 0
00533300 8d45f4                   lea        eax, [ebp - 0xc]
00533303 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00533306 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00533309 8b11                     mov        edx, dword ptr [ecx]
0053330b 52                       push       edx
0053330c 8d45f8                   lea        eax, [ebp - 8]
0053330f 50                       push       eax
00533310 e8fbe1f2ff               call       0x461510
00533315 83c408                   add        esp, 8
00533318 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0053331b 8b11                     mov        edx, dword ptr [ecx]
0053331d 52                       push       edx
0053331e 8d45f0                   lea        eax, [ebp - 0x10]
00533321 50                       push       eax
00533322 e8a9e2f2ff               call       0x4615d0
00533327 83c408                   add        esp, 8
0053332a eb09                     jmp        0x533335
0053332c 8d4df8                   lea        ecx, [ebp - 8]
0053332f e82ce5f2ff               call       0x461860
00533334 90                       nop        
00533335 8d4df0                   lea        ecx, [ebp - 0x10]
00533338 51                       push       ecx
00533339 8d4df8                   lea        ecx, [ebp - 8]
0053333c e81f61f1ff               call       0x449460
00533341 0fb6d0                   movzx      edx, al
00533344 85d2                     test       edx, edx
00533346 7444                     je         0x53338c
00533348 8d4df8                   lea        ecx, [ebp - 8]
0053334b e8b08fedff               call       0x40c300
00533350 8945e0                   mov        dword ptr [ebp - 0x20], eax
00533353 6a00                     push       0
00533355 e8260df3ff               call       0x464080
0053335a 83c404                   add        esp, 4
0053335d 8945dc                   mov        dword ptr [ebp - 0x24], eax
00533360 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
00533366 e8a5e4eeff               call       0x421810
0053336b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0053336e 8a9408ec000000           mov        dl, byte ptr [eax + ecx + 0xec]
00533375 8855ef                   mov        byte ptr [ebp - 0x11], dl
00533378 0fb645ef                 movzx      eax, byte ptr [ebp - 0x11]
0053337c 50                       push       eax
0053337d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00533380 51                       push       ecx
00533381 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00533384 e8a7130000               call       0x534730
00533389 90                       nop        
0053338a eba0                     jmp        0x53332c
0053338c 6a00                     push       0
0053338e e89d0ef3ff               call       0x464230
00533393 83c404                   add        esp, 4
00533396 8bc8                     mov        ecx, eax
00533398 e813000000               call       0x5333b0
0053339d 90                       nop        
0053339e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005333a1 33cd                     xor        ecx, ebp
005333a3 e846f60000               call       0x5429ee
005333a8 8be5                     mov        esp, ebp
005333aa 5d                       pop        ebp
005333ab c3                       ret        
005333ac cc                       int3       
005333ad cc                       int3       
005333ae cc                       int3       
005333af cc                       int3       
