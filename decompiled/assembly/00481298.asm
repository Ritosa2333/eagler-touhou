
// block 00481298
00481298  mov        edi, edi
0048129a  push       ebp
0048129b  mov        ebp, esp
0048129d  sub        esp, 0x24
004812a0  push       ebx
004812a1  push       esi
004812a2  mov        esi, 0x2000
004812a7  push       edi
004812a8  test       dword ptr [0x4b4328], esi
004812ae  je         0x4812e0

// block 004812b0
004812b0  and        dword ptr [0x4b4328], 0xffffdfff
004812ba  lea        eax, [ebp - 0x14]
004812bd  push       0
004812bf  push       eax
004812c0  call       0x4827df

// block 004812c5
004812c5  or         dword ptr [0x4b4328], esi
004812cb  pop        ecx
004812cc  pop        ecx

// block 004812cd
004812cd  mov        ecx, dword ptr [ebp - 0x14]
004812d0  mov        eax, dword ptr [ebp + 8]
004812d3  mov        dword ptr [eax], ecx
004812d5  mov        ecx, dword ptr [ebp - 0x10]
004812d8  mov        dword ptr [eax + 4], ecx
004812db  jmp        0x4814ab

// block 004812e0
004812e0  mov        eax, dword ptr [0x4b4318]
004812e5  mov        cl, byte ptr [eax]
004812e7  cmp        cl, 0x3f
004812ea  jne        0x48149a

// block 004812f0
004812f0  inc        eax
004812f1  mov        dword ptr [0x4b4318], eax
004812f6  cmp        byte ptr [eax], cl
004812f8  jne        0x48131d

// block 004812fa
004812fa  cmp        byte ptr [eax + 1], cl
004812fd  jne        0x48131d

// block 004812ff
004812ff  lea        eax, [ebp - 0x14]
00481302  push       eax
00481303  call       0x481298

// block 00481308
00481308  mov        eax, dword ptr [0x4b4318]
0048130d  pop        ecx
0048130e  jmp        0x481316

// block 00481310
00481310  inc        eax
00481311  mov        dword ptr [0x4b4318], eax

// block 00481316
00481316  cmp        byte ptr [eax], 0
00481319  jne        0x481310

// block 0048131b
0048131b  jmp        0x4812cd

// block 0048131d
0048131d  lea        eax, [ebp - 0xc]
00481320  push       eax
00481321  call       0x48061b

// block 00481326
00481326  mov        esi, dword ptr [ebp - 0xc]
00481329  mov        ebx, dword ptr [ebp - 8]
0048132c  xor        eax, eax
0048132e  inc        eax
0048132f  pop        ecx
00481330  test       esi, esi
00481332  je         0x481341

// block 00481334
00481334  test       ebx, 0x200
0048133a  je         0x481341

// block 0048133c
0048133c  mov        dword ptr [ebp - 4], eax
0048133f  jmp        0x481345

// block 00481341
00481341  and        dword ptr [ebp - 4], 0

// block 00481345
00481345  mov        edi, ebx
00481347  shr        edi, 0xf
0048134a  and        edi, eax
0048134c  cmp        bl, al
0048134e  jle        0x48135d

// block 00481350
00481350  mov        eax, dword ptr [ebp + 8]
00481353  mov        dword ptr [eax], esi
00481355  mov        dword ptr [eax + 4], ebx
00481358  jmp        0x4814ab

// block 0048135d
0048135d  mov        eax, dword ptr [0x4b4318]
00481362  mov        al, byte ptr [eax]
00481364  test       al, al
00481366  je         0x48140a

// block 0048136c
0048136c  cmp        al, 0x40
0048136e  je         0x48140a

// block 00481374
00481374  lea        eax, [ebp - 0x14]
00481377  push       eax
00481378  call       0x4814b0

// block 0048137d
0048137d  cmp        dword ptr [ebp - 0x14], 0
00481381  pop        ecx
00481382  je         0x48140a

// block 00481388
00481388  cmp        byte ptr [0x4b4330], 0
0048138f  je         0x4813df

// block 00481391
00481391  lea        eax, [ebp - 0x14]
00481394  push       eax
00481395  lea        eax, [ebp - 0x1c]
00481398  push       eax
00481399  lea        ecx, [ebp - 0xc]
0048139c  mov        byte ptr [0x4b4330], 0
004813a3  call       0x47e9ae

// block 004813a8
004813a8  mov        esi, dword ptr [eax]
004813aa  mov        ebx, dword ptr [eax + 4]
004813ad  mov        eax, dword ptr [0x4b4318]
004813b2  cmp        byte ptr [eax], 0x40
004813b5  mov        dword ptr [ebp - 0xc], esi
004813b8  mov        dword ptr [ebp - 8], ebx
004813bb  je         0x48140a

// block 004813bd
004813bd  lea        eax, [ebp - 0x1c]
004813c0  push       eax
004813c1  call       0x4814b0

// block 004813c6
004813c6  pop        ecx
004813c7  mov        ecx, dword ptr [eax]
004813c9  mov        dword ptr [ebp - 0x14], ecx
004813cc  mov        eax, dword ptr [eax + 4]
004813cf  mov        dword ptr [ebp - 0x10], eax
004813d2  lea        eax, [ebp - 0xc]
004813d5  push       eax
004813d6  lea        eax, [ebp - 0x1c]
004813d9  push       eax
004813da  lea        eax, [ebp - 0x24]
004813dd  jmp        0x4813ea

// block 004813df
004813df  lea        eax, [ebp - 0xc]
004813e2  push       eax
004813e3  lea        eax, [ebp - 0x24]
004813e6  push       eax
004813e7  lea        eax, [ebp - 0x1c]

// block 004813ea
004813ea  push       0x49df1c
004813ef  push       eax
004813f0  lea        ecx, [ebp - 0x14]
004813f3  call       0x47ec92

// block 004813f8
004813f8  mov        ecx, eax
004813fa  call       0x47e9ae

// block 004813ff
004813ff  mov        ebx, dword ptr [eax + 4]
00481402  mov        esi, dword ptr [eax]
00481404  mov        dword ptr [ebp - 8], ebx
00481407  mov        dword ptr [ebp - 0xc], esi

// block 0048140a
0048140a  cmp        dword ptr [ebp - 4], 0
0048140e  je         0x48141d

// block 00481410
00481410  test       esi, esi
00481412  je         0x48141d

// block 00481414
00481414  or         ebx, 0x200
0048141a  mov        dword ptr [ebp - 8], ebx

// block 0048141d
0048141d  mov        edx, 0x8000
00481422  test       edi, edi
00481424  je         0x48142b

// block 00481426
00481426  or         ebx, edx
00481428  mov        dword ptr [ebp - 8], ebx

// block 0048142b
0048142b  test       esi, esi
0048142d  je         0x481350

// block 00481433
00481433  mov        ecx, 0x1000
00481438  test       ecx, ebx
0048143a  jne        0x481350

// block 00481440
00481440  mov        eax, dword ptr [0x4b4318]
00481445  mov        al, byte ptr [eax]
00481447  test       al, al
00481449  je         0x481459

// block 0048144b
0048144b  cmp        al, 0x40
0048144d  je         0x481453

// block 0048144f
0048144f  push       2
00481451  jmp        0x4814a0

// block 00481453
00481453  inc        dword ptr [0x4b4318]

// block 00481459
00481459  test       dword ptr [0x4b4328], ecx
0048145f  je         0x48148a

// block 00481461
00481461  cmp        dword ptr [ebp - 4], 0
00481465  jne        0x48148a

// block 00481467
00481467  test       edx, ebx
00481469  jne        0x48148a

// block 0048146b
0048146b  and        dword ptr [ebp - 0x14], 0
0048146f  and        dword ptr [ebp - 0x10], 0xffff0000
00481476  lea        eax, [ebp - 0x14]
00481479  push       eax
0048147a  lea        eax, [ebp - 0x24]
0048147d  push       eax
0048147e  call       0x4806fd

// block 00481483
00481483  pop        ecx
00481484  pop        ecx
00481485  jmp        0x481350

// block 0048148a
0048148a  lea        eax, [ebp - 0xc]
0048148d  push       eax
0048148e  push       dword ptr [ebp + 8]
00481491  call       0x4806fd

// block 00481496
00481496  pop        ecx
00481497  pop        ecx
00481498  jmp        0x4814a8

// block 0048149a
0048149a  test       cl, cl
0048149c  jne        0x48144f

// block 0048149e
0048149e  push       1

// block 004814a0
004814a0  mov        ecx, dword ptr [ebp + 8]
004814a3  call       0x47e1ce

// block 004814a8
004814a8  mov        eax, dword ptr [ebp + 8]

// block 004814ab
004814ab  pop        edi
004814ac  pop        esi
004814ad  pop        ebx
004814ae  leave      
004814af  ret        
