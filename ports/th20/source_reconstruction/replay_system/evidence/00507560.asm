00507560 55                       push       ebp
00507561 8bec                     mov        ebp, esp
00507563 6aff                     push       -1
00507565 689d985600               push       0x56989d
0050756a 64a100000000             mov        eax, dword ptr fs:[0]
00507570 50                       push       eax
00507571 51                       push       ecx
00507572 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00507577 33c5                     xor        eax, ebp
00507579 50                       push       eax
0050757a 8d45f4                   lea        eax, [ebp - 0xc]
0050757d 64a300000000             mov        dword ptr fs:[0], eax
00507583 894df0                   mov        dword ptr [ebp - 0x10], ecx
00507586 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00507589 e89287f1ff               call       0x41fd20
0050758e c745fc00000000           mov        dword ptr [ebp - 4], 0
00507595 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00507598 c700e4325700             mov        dword ptr [eax], 0x5732e4
0050759e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005075a1 c7411000000000           mov        dword ptr [ecx + 0x10], 0
005075a8 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005075ab c7421400000000           mov        dword ptr [edx + 0x14], 0
005075b2 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005075b5 c7401800000000           mov        dword ptr [eax + 0x18], 0
005075bc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005075bf c7411c00000000           mov        dword ptr [ecx + 0x1c], 0
005075c6 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005075c9 c7422000000000           mov        dword ptr [edx + 0x20], 0
005075d0 33c0                     xor        eax, eax
005075d2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005075d5 83c124                   add        ecx, 0x24
005075d8 8901                     mov        dword ptr [ecx], eax
005075da 894104                   mov        dword ptr [ecx + 4], eax
005075dd 894108                   mov        dword ptr [ecx + 8], eax
005075e0 89410c                   mov        dword ptr [ecx + 0xc], eax
005075e3 894110                   mov        dword ptr [ecx + 0x10], eax
005075e6 894114                   mov        dword ptr [ecx + 0x14], eax
005075e9 894118                   mov        dword ptr [ecx + 0x18], eax
005075ec 68108a4100               push       0x418a10
005075f1 6a08                     push       8
005075f3 6a14                     push       0x14
005075f5 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005075f8 83c240                   add        edx, 0x40
005075fb 52                       push       edx
005075fc e81f46f0ff               call       0x40bc20
00507601 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00507604 c780e000000000000000     mov        dword ptr [eax + 0xe0], 0
0050760e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00507611 c781e400000000000000     mov        dword ptr [ecx + 0xe4], 0
0050761b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050761e 81c1e8000000             add        ecx, 0xe8
00507624 e807fdffff               call       0x507330
00507629 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0050762c c7824802000000000000     mov        dword ptr [edx + 0x248], 0
00507636 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00507639 c6804c02000000           mov        byte ptr [eax + 0x24c], 0
00507640 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00507643 c7815002000000000000     mov        dword ptr [ecx + 0x250], 0
0050764d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00507650 c7825402000000000000     mov        dword ptr [edx + 0x254], 0
0050765a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0050765d c7805802000000000000     mov        dword ptr [eax + 0x258], 0
00507667 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050766a 81c15c020000             add        ecx, 0x25c
00507670 e80bfcffff               call       0x507280
00507675 6800010000               push       0x100
0050767a 6a00                     push       0
0050767c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050767f 81c160020000             add        ecx, 0x260
00507685 51                       push       ecx
00507686 e8d5d00300               call       0x544760
0050768b 83c40c                   add        esp, 0xc
0050768e c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00507695 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00507698 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0050769b 64890d00000000           mov        dword ptr fs:[0], ecx
005076a2 59                       pop        ecx
005076a3 8be5                     mov        esp, ebp
005076a5 5d                       pop        ebp
005076a6 c3                       ret        
005076a7 cc                       int3       
005076a8 cc                       int3       
005076a9 cc                       int3       
005076aa cc                       int3       
005076ab cc                       int3       
005076ac cc                       int3       
005076ad cc                       int3       
005076ae cc                       int3       
005076af cc                       int3       
