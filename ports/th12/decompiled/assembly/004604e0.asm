
// block 004604e0
004604e0  push       ebx
004604e1  xor        ebx, ebx
004604e3  cmp        dword ptr [edi + 0x108], ebx
004604e9  je         0x4605d1

// block 004604ef
004604ef  push       ebp
004604f0  push       esi
004604f1  mov        esi, dword ptr [0x4ce8cc]
004604f7  mov        edx, edi
004604f9  call       0x461be0

// block 004604fe
004604fe  xor        ebp, ebp
00460500  cmp        dword ptr [edi + 0x10c], ebx
00460506  jle        0x460552

// block 00460508
00460508  jmp        0x460510

// block 00460510
00460510  mov        esi, dword ptr [edi + 0x120]
00460516  mov        eax, dword ptr [esi + ebx]
00460519  add        esi, ebx
0046051b  test       eax, eax
0046051d  je         0x46052d

// block 0046051f
0046051f  mov        ecx, dword ptr [eax]
00460521  mov        edx, dword ptr [ecx + 8]
00460524  push       eax
00460525  call       edx

// block 00460527
00460527  mov        dword ptr [esi], 0

// block 0046052d
0046052d  mov        eax, dword ptr [esi + 4]
00460530  test       eax, eax
00460532  je         0x460544

// block 00460534
00460534  push       eax
00460535  call       0x46c941

// block 0046053a
0046053a  add        esp, 4
0046053d  mov        dword ptr [esi + 4], 0

// block 00460544
00460544  inc        ebp
00460545  add        ebx, 0x14
00460548  cmp        ebp, dword ptr [edi + 0x10c]
0046054e  jl         0x460510

// block 00460550
00460550  xor        ebx, ebx

// block 00460552
00460552  mov        eax, dword ptr [edi + 0x120]
00460558  pop        esi
00460559  pop        ebp
0046055a  cmp        eax, ebx
0046055c  je         0x46056d

// block 0046055e
0046055e  push       eax
0046055f  call       0x46c941

// block 00460564
00460564  add        esp, 4
00460567  mov        dword ptr [edi + 0x120], ebx

// block 0046056d
0046056d  mov        eax, dword ptr [edi + 0x118]
00460573  cmp        eax, ebx
00460575  je         0x460586

// block 00460577
00460577  push       eax
00460578  call       0x46c941

// block 0046057d
0046057d  add        esp, 4
00460580  mov        dword ptr [edi + 0x118], ebx

// block 00460586
00460586  mov        eax, dword ptr [edi + 0x11c]
0046058c  cmp        eax, ebx
0046058e  je         0x46059f

// block 00460590
00460590  push       eax
00460591  call       0x46c941

// block 00460596
00460596  add        esp, 4
00460599  mov        dword ptr [edi + 0x11c], ebx

// block 0046059f
0046059f  mov        eax, dword ptr [edi + 0x134]
004605a5  cmp        eax, ebx
004605a7  je         0x4605b8

// block 004605a9
004605a9  push       eax
004605aa  call       0x46c941

// block 004605af
004605af  add        esp, 4
004605b2  mov        dword ptr [edi + 0x134], ebx

// block 004605b8
004605b8  mov        eax, dword ptr [edi + 0x108]
004605be  cmp        eax, ebx
004605c0  je         0x4605d1

// block 004605c2
004605c2  push       eax
004605c3  call       0x46c941

// block 004605c8
004605c8  add        esp, 4
004605cb  mov        dword ptr [edi + 0x108], ebx

// block 004605d1
004605d1  pop        ebx
004605d2  ret        
