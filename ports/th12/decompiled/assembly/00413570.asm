
// block 00413570
00413570  push       -1
00413572  push       0x497408
00413577  mov        eax, dword ptr fs:[0]
0041357d  push       eax
0041357e  push       ecx
0041357f  push       ebx
00413580  push       ebp
00413581  push       esi
00413582  push       edi
00413583  mov        eax, dword ptr [0x4ad138]
00413588  xor        eax, esp
0041358a  push       eax
0041358b  lea        eax, [esp + 0x18]
0041358f  mov        dword ptr fs:[0], eax
00413595  mov        esi, dword ptr [esp + 0x28]
00413599  mov        dword ptr [esi], 0x49fc50
0041359f  xor        ebx, ebx
004135a1  mov        edx, esi
004135a3  mov        dword ptr [esp + 0x20], ebx
004135a7  call       0x412be0

// block 004135ac
004135ac  test       dword ptr [esi + 0x26f8], 0x400000
004135b6  je         0x4135c8

// block 004135b8
004135b8  mov        eax, dword ptr [esi + 0x2704]
004135be  mov        ecx, dword ptr [0x4b43dc]
004135c4  mov        dword ptr [ecx + eax*4 + 0x1c], ebx

// block 004135c8
004135c8  mov        ebp, dword ptr [0x4ce8cc]
004135ce  add        esi, 0x1120
004135d4  mov        dword ptr [esp + 0x14], 0x10
004135dc  mov        edi, 0x10000000

// block 004135e1
004135e1  mov        edx, dword ptr [esi]
004135e3  cmp        edx, ebx
004135e5  je         0x41363f

// block 004135e7
004135e7  mov        eax, dword ptr [ebp + 0x8856b8]
004135ed  cmp        eax, ebx
004135ef  je         0x4135fe

// block 004135f1
004135f1  mov        ecx, dword ptr [eax]
004135f3  cmp        dword ptr [ecx], edx
004135f5  je         0x413617

// block 004135f7
004135f7  mov        eax, dword ptr [eax + 4]
004135fa  cmp        eax, ebx
004135fc  jne        0x4135f1

// block 004135fe
004135fe  mov        eax, dword ptr [ebp + 0x8856c0]
00413604  cmp        eax, ebx
00413606  je         0x41363f

// block 00413608
00413608  mov        ecx, dword ptr [eax]
0041360a  cmp        dword ptr [ecx], edx
0041360c  je         0x413617

// block 0041360e
0041360e  mov        eax, dword ptr [eax + 4]
00413611  cmp        eax, ebx
00413613  jne        0x413608

// block 00413615
00413615  jmp        0x41363f

// block 00413617
00413617  mov        eax, ecx
00413619  cmp        eax, ebx
0041361b  je         0x41363f

// block 0041361d
0041361d  or         dword ptr [eax + 0x47c], edi
00413623  cmp        dword ptr [eax + 0x18], ebx
00413626  jne        0x41363f

// block 00413628
00413628  mov        eax, dword ptr [eax + 0x14]
0041362b  cmp        eax, ebx
0041362d  je         0x41363f

// block 0041362f
0041362f  nop        

// block 00413630
00413630  mov        ecx, dword ptr [eax]
00413632  or         dword ptr [ecx + 0x47c], edi
00413638  mov        eax, dword ptr [eax + 4]
0041363b  cmp        eax, ebx
0041363d  jne        0x413630

// block 0041363f
0041363f  mov        dword ptr [esi], ebx
00413641  add        esi, 4
00413644  sub        dword ptr [esp + 0x14], 1
00413649  jne        0x4135e1

// block 0041364b
0041364b  mov        eax, dword ptr [0x4b4514]
00413650  mov        edi, dword ptr [esp + 0x28]
00413654  cmp        eax, ebx
00413656  je         0x413684

// block 00413658
00413658  cmp        dword ptr [eax + 0x8980], edi
0041365e  jne        0x41366c

// block 00413660
00413660  mov        dword ptr [eax + 0x8980], ebx
00413666  mov        byte ptr [eax + 0x8984], bl

// block 0041366c
0041366c  add        eax, 0xaac
00413671  mov        ecx, 0x100

// block 00413676
00413676  cmp        dword ptr [eax], edi
00413678  jne        0x41367c

// block 0041367a
0041367a  mov        dword ptr [eax], ebx

// block 0041367c
0041367c  add        eax, 0x78
0041367f  sub        ecx, 1
00413682  jne        0x413676

// block 00413684
00413684  mov        esi, dword ptr [edi + 0x2790]
0041368a  cmp        esi, ebx
0041368c  je         0x41369c

// block 0041368e
0041368e  call       0x402870

// block 00413693
00413693  push       esi
00413694  call       0x46ca4f

// block 00413699
00413699  add        esp, 4

// block 0041369c
0041369c  mov        dword ptr [esp + 0x20], 0xffffffff
004136a4  mov        esi, dword ptr [edi + 0x1034]
004136aa  mov        dword ptr [edi + 0x2790], ebx
004136b0  mov        dword ptr [edi], 0x49fc34
004136b6  cmp        esi, ebx
004136b8  je         0x4136da

// block 004136ba
004136ba  lea        ebx, [ebx]

// block 004136c0
004136c0  mov        edx, dword ptr [esi]
004136c2  mov        edi, dword ptr [esi + 4]
004136c5  push       edx
004136c6  call       0x46ca4f

// block 004136cb
004136cb  push       esi
004136cc  call       0x46ca4f

// block 004136d1
004136d1  add        esp, 8
004136d4  mov        esi, edi
004136d6  cmp        edi, ebx
004136d8  jne        0x4136c0

// block 004136da
004136da  mov        ecx, dword ptr [esp + 0x18]
004136de  mov        dword ptr fs:[0], ecx
004136e5  pop        ecx
004136e6  pop        edi
004136e7  pop        esi
004136e8  pop        ebp
004136e9  pop        ebx
004136ea  add        esp, 0x10
004136ed  ret        4
