00507360 55                       push       ebp
00507361 8bec                     mov        ebp, esp
00507363 51                       push       ecx
00507364 894dfc                   mov        dword ptr [ebp - 4], ecx
00507367 33c0                     xor        eax, eax
00507369 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050736c 8901                     mov        dword ptr [ecx], eax
0050736e 894104                   mov        dword ptr [ecx + 4], eax
00507371 884108                   mov        byte ptr [ecx + 8], al
00507374 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507377 c6420900                 mov        byte ptr [edx + 9], 0
0050737b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050737e 83c10a                   add        ecx, 0xa
00507381 e8aafeffff               call       0x507230
00507386 33c0                     xor        eax, eax
00507388 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050738b 83c10b                   add        ecx, 0xb
0050738e 8801                     mov        byte ptr [ecx], al
00507390 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507393 33c0                     xor        eax, eax
00507395 33c9                     xor        ecx, ecx
00507397 894210                   mov        dword ptr [edx + 0x10], eax
0050739a 894a14                   mov        dword ptr [edx + 0x14], ecx
0050739d 8b55fc                   mov        edx, dword ptr [ebp - 4]
005073a0 33c0                     xor        eax, eax
005073a2 33c9                     xor        ecx, ecx
005073a4 894218                   mov        dword ptr [edx + 0x18], eax
005073a7 894a1c                   mov        dword ptr [edx + 0x1c], ecx
005073aa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005073ad 83c120                   add        ecx, 0x20
005073b0 e85b28fbff               call       0x4b9c10
005073b5 8b55fc                   mov        edx, dword ptr [ebp - 4]
005073b8 0f57c0                   xorps      xmm0, xmm0
005073bb f30f1182d0000000         movss      dword ptr [edx + 0xd0], xmm0
005073c3 8b45fc                   mov        eax, dword ptr [ebp - 4]
005073c6 c780d400000000000000     mov        dword ptr [eax + 0xd4], 0
005073d0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005073d3 c781d800000000000000     mov        dword ptr [ecx + 0xd8], 0
005073dd 8b55fc                   mov        edx, dword ptr [ebp - 4]
005073e0 c782dc00000000000000     mov        dword ptr [edx + 0xdc], 0
005073ea 8b45fc                   mov        eax, dword ptr [ebp - 4]
005073ed c780e000000008000000     mov        dword ptr [eax + 0xe0], 8
005073f7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005073fa c781e400000008000000     mov        dword ptr [ecx + 0xe4], 8
00507404 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507407 c782e800000008000000     mov        dword ptr [edx + 0xe8], 8
00507411 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507414 c680ec00000000           mov        byte ptr [eax + 0xec], 0
0050741b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050741e c681ed00000000           mov        byte ptr [ecx + 0xed], 0
00507425 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507428 c682ee00000000           mov        byte ptr [edx + 0xee], 0
0050742f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507432 c680ef00000000           mov        byte ptr [eax + 0xef], 0
00507439 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050743c c781f000000001000000     mov        dword ptr [ecx + 0xf0], 1
00507446 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507449 c782f400000000000000     mov        dword ptr [edx + 0xf4], 0
00507453 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507456 c780f800000000000000     mov        dword ptr [eax + 0xf8], 0
00507460 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507463 c781fc00000000000000     mov        dword ptr [ecx + 0xfc], 0
0050746d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507470 8be5                     mov        esp, ebp
00507472 5d                       pop        ebp
00507473 c3                       ret        
00507474 cc                       int3       
00507475 cc                       int3       
00507476 cc                       int3       
00507477 cc                       int3       
00507478 cc                       int3       
00507479 cc                       int3       
0050747a cc                       int3       
0050747b cc                       int3       
0050747c cc                       int3       
0050747d cc                       int3       
0050747e cc                       int3       
0050747f cc                       int3       
