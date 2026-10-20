
// block 00461450
00461450  push       ebx
00461451  push       ebp
00461452  mov        ebp, dword ptr [esp + 0xc]
00461456  push       edi
00461457  lea        edi, [esi + 4]
0046145a  cmp        edi, dword ptr [ebp + 0x8856bc]
00461460  jne        0x46146b

// block 00461462
00461462  mov        eax, dword ptr [esi + 0xc]
00461465  mov        dword ptr [ebp + 0x8856bc], eax

// block 0046146b
0046146b  cmp        edi, dword ptr [ebp + 0x8856b8]
00461471  jne        0x46147c

// block 00461473
00461473  mov        ecx, dword ptr [esi + 8]
00461476  mov        dword ptr [ebp + 0x8856b8], ecx

// block 0046147c
0046147c  cmp        edi, dword ptr [ebp + 0x8856c4]
00461482  jne        0x46148d

// block 00461484
00461484  mov        edx, dword ptr [esi + 0xc]
00461487  mov        dword ptr [ebp + 0x8856c4], edx

// block 0046148d
0046148d  cmp        edi, dword ptr [ebp + 0x8856c0]
00461493  jne        0x46149e

// block 00461495
00461495  mov        eax, dword ptr [esi + 8]
00461498  mov        dword ptr [ebp + 0x8856c0], eax

// block 0046149e
0046149e  mov        eax, dword ptr [esi + 0x490]
004614a4  xor        ebx, ebx
004614a6  cmp        eax, ebx
004614a8  je         0x4614ae

// block 004614aa
004614aa  mov        ecx, esi
004614ac  call       eax

// block 004614ae
004614ae  mov        eax, dword ptr [edi + 4]
004614b1  cmp        eax, ebx
004614b3  je         0x4614bb

// block 004614b5
004614b5  mov        ecx, dword ptr [edi + 8]
004614b8  mov        dword ptr [eax + 8], ecx

// block 004614bb
004614bb  mov        eax, dword ptr [edi + 8]
004614be  cmp        eax, ebx
004614c0  je         0x4614c8

// block 004614c2
004614c2  mov        edx, dword ptr [edi + 4]
004614c5  mov        dword ptr [eax + 4], edx

// block 004614c8
004614c8  mov        dword ptr [edi + 4], ebx
004614cb  mov        dword ptr [edi + 8], ebx
004614ce  cmp        dword ptr [esi + 0x18], ebx
004614d1  jne        0x4614f4

// block 004614d3
004614d3  mov        eax, dword ptr [esi + 0x14]
004614d6  cmp        eax, ebx
004614d8  je         0x4614f4

// block 004614da
004614da  mov        edx, 0x10000000
004614df  nop        

// block 004614e0
004614e0  mov        ecx, dword ptr [eax]
004614e2  mov        dword ptr [ecx + 0x3c], ebx
004614e5  mov        ecx, dword ptr [eax]
004614e7  or         dword ptr [ecx + 0x47c], edx
004614ed  mov        eax, dword ptr [eax + 4]
004614f0  cmp        eax, ebx
004614f2  jne        0x4614e0

// block 004614f4
004614f4  mov        eax, dword ptr [esi + 0x14]
004614f7  cmp        eax, ebx
004614f9  je         0x461501

// block 004614fb
004614fb  mov        edx, dword ptr [esi + 0x18]
004614fe  mov        dword ptr [eax + 8], edx

// block 00461501
00461501  mov        eax, dword ptr [esi + 0x18]
00461504  cmp        eax, ebx
00461506  je         0x46150e

// block 00461508
00461508  mov        ecx, dword ptr [esi + 0x14]
0046150b  mov        dword ptr [eax + 4], ecx

// block 0046150e
0046150e  lea        edx, [ebp + 0xbc]
00461514  mov        dword ptr [esi + 0x14], ebx
00461517  mov        dword ptr [esi + 0x18], ebx
0046151a  cmp        esi, edx
0046151c  jb         0x46156b

// block 0046151e
0046151e  lea        eax, [ebp + 0x4b40bc]
00461524  cmp        esi, eax
00461526  jae        0x46156b

// block 00461528
00461528  mov        ecx, esi
0046152a  sub        ecx, ebp
0046152c  sub        ecx, 0xbc
00461532  mov        eax, 0x1b37484b
00461537  imul       ecx
00461539  sar        edx, 7
0046153c  mov        ecx, edx
0046153e  shr        ecx, 0x1f
00461541  add        ecx, edx
00461543  mov        byte ptr [ecx + ebp + 0x4b40bc], bl
0046154a  mov        eax, dword ptr [esi + 0x478]
00461550  cmp        eax, ebx
00461552  je         0x46155d

// block 00461554
00461554  push       eax
00461555  call       0x46c941

// block 0046155a
0046155a  add        esp, 4

// block 0046155d
0046155d  pop        edi
0046155e  pop        ebp
0046155f  mov        dword ptr [esi + 0x478], ebx
00461565  xor        eax, eax
00461567  pop        ebx
00461568  ret        4

// block 0046156b
0046156b  mov        eax, dword ptr [esi + 0x478]
00461571  cmp        eax, ebx
00461573  je         0x46157e

// block 00461575
00461575  push       eax
00461576  call       0x46c941

// block 0046157b
0046157b  add        esp, 4

// block 0046157e
0046157e  push       esi
0046157f  mov        dword ptr [esi + 0x478], ebx
00461585  call       0x46ca4f

// block 0046158a
0046158a  add        esp, 4
0046158d  pop        edi
0046158e  pop        ebp
0046158f  xor        eax, eax
00461591  pop        ebx
00461592  ret        4
