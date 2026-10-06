
// block 0048023a
0048023a  mov        edi, edi
0048023c  push       ebp
0048023d  mov        ebp, esp
0048023f  sub        esp, 0x38
00480242  mov        eax, dword ptr [0x4ad138]
00480247  xor        eax, ebp
00480249  mov        dword ptr [ebp - 4], eax
0048024c  mov        eax, dword ptr [0x4b4318]
00480251  push       ebx
00480252  mov        bl, byte ptr [eax]
00480254  movsx      eax, bl
00480257  push       esi
00480258  mov        esi, dword ptr [ebp + 8]
0048025b  sub        eax, 0x30
0048025e  mov        dword ptr [ebp - 0x30], esi
00480261  cmp        eax, 9
00480264  ja         0x480280

// block 00480266
00480266  mov        ecx, dword ptr [0x4b4310]
0048026c  inc        dword ptr [0x4b4318]
00480272  push       eax
00480273  push       esi
00480274  call       0x47e378

// block 00480279
00480279  mov        eax, esi
0048027b  jmp        0x480422

// block 00480280
00480280  and        dword ptr [ebp - 0x1c], 0
00480284  mov        esi, 0xffff0000
00480289  and        dword ptr [ebp - 0x18], esi
0048028c  cmp        bl, 0x3f
0048028f  jne        0x4802db

// block 00480291
00480291  lea        eax, [ebp - 0x24]
00480294  push       0
00480296  push       eax
00480297  call       0x4800f3

// block 0048029c
0048029c  pop        ecx
0048029d  pop        ecx
0048029e  mov        ecx, dword ptr [eax]
004802a0  mov        eax, dword ptr [eax + 4]
004802a3  mov        dword ptr [ebp - 0x18], eax
004802a6  mov        eax, dword ptr [0x4b4318]
004802ab  mov        dword ptr [ebp - 0x1c], ecx
004802ae  mov        cl, byte ptr [eax]
004802b0  inc        eax
004802b1  mov        dword ptr [0x4b4318], eax
004802b6  cmp        cl, 0x40
004802b9  je         0x4803fa

// block 004802bf
004802bf  dec        eax
004802c0  xor        ecx, ecx
004802c2  mov        dword ptr [0x4b4318], eax
004802c7  cmp        byte ptr [eax], cl
004802c9  setne      cl
004802cc  inc        ecx
004802cd  push       ecx
004802ce  lea        ecx, [ebp - 0x1c]
004802d1  call       0x47e2f7

// block 004802d6
004802d6  jmp        0x4803fa

// block 004802db
004802db  mov        ecx, dword ptr [0x4b4318]
004802e1  push       edi
004802e2  mov        edi, 0x49df08
004802e7  push       0x13
004802e9  mov        edx, edi
004802eb  call       0x47e029

// block 004802f0
004802f0  pop        ecx
004802f1  test       eax, eax
004802f3  jne        0x4802fe

// block 004802f5
004802f5  add        dword ptr [0x4b4318], 0x13
004802fc  jmp        0x480322

// block 004802fe
004802fe  mov        ecx, dword ptr [0x4b4318]
00480304  mov        edi, 0x49def8
00480309  push       0xd
0048030b  mov        edx, edi
0048030d  call       0x47e029

// block 00480312
00480312  pop        ecx
00480313  test       eax, eax
00480315  jne        0x4803bf

// block 0048031b
0048031b  add        dword ptr [0x4b4318], 0xd

// block 00480322
00480322  lea        eax, [ebp - 0x24]
00480325  push       eax
00480326  call       0x47f57e

// block 0048032b
0048032b  test       dword ptr [0x4b4328], 0x4000
00480335  pop        ecx
00480336  je         0x480386

// block 00480338
00480338  push       0x10
0048033a  lea        eax, [ebp - 0x14]
0048033d  push       eax
0048033e  lea        ecx, [ebp - 0x24]
00480341  call       0x47e213

// block 00480346
00480346  lea        eax, [ebp - 0x14]
00480349  push       eax
0048034a  call       0x46d114

// block 0048034f
0048034f  push       eax
00480350  call       dword ptr [0x4b432c]

// block 00480356
00480356  pop        ecx
00480357  pop        ecx
00480358  lea        ecx, [ebp - 0x1c]
0048035b  test       eax, eax
0048035d  je         0x48036a

// block 0048035f
0048035f  push       eax
00480360  call       0x47e896

// block 00480365
00480365  jmp        0x4803f9

// block 0048036a
0048036a  push       0x49def4
0048036f  call       0x47e896

// block 00480374
00480374  push       0x49dea8
00480379  lea        eax, [ebp - 0x38]
0048037c  push       eax
0048037d  lea        eax, [ebp - 0x24]
00480380  push       eax
00480381  lea        eax, [ebp - 0x2c]
00480384  jmp        0x4803a3

// block 00480386
00480386  push       0x49def4
0048038b  lea        ecx, [ebp - 0x1c]
0048038e  call       0x47e896

// block 00480393
00480393  push       0x49dea8
00480398  lea        eax, [ebp - 0x2c]
0048039b  push       eax
0048039c  lea        eax, [ebp - 0x24]
0048039f  push       eax
004803a0  lea        eax, [ebp - 0x38]

// block 004803a3
004803a3  push       edi
004803a4  push       eax
004803a5  call       0x47ec4a

// block 004803aa
004803aa  add        esp, 0xc
004803ad  mov        ecx, eax
004803af  call       0x47ec92

// block 004803b4
004803b4  push       eax
004803b5  lea        ecx, [ebp - 0x1c]
004803b8  call       0x47e7bf

// block 004803bd
004803bd  jmp        0x4803f9

// block 004803bf
004803bf  cmp        byte ptr [ebp + 0x10], 0
004803c3  je         0x4803df

// block 004803c5
004803c5  cmp        bl, 0x40
004803c8  jne        0x4803df

// block 004803ca
004803ca  mov        ecx, dword ptr [ebp - 0x28]
004803cd  xor        eax, eax
004803cf  and        ecx, esi
004803d1  inc        dword ptr [0x4b4318]
004803d7  mov        dword ptr [ebp - 0x1c], eax
004803da  mov        dword ptr [ebp - 0x18], ecx
004803dd  jmp        0x4803f9

// block 004803df
004803df  push       0x40
004803e1  push       0x4b4318
004803e6  lea        ecx, [ebp - 0x2c]
004803e9  call       0x47e5d3

// block 004803ee
004803ee  mov        ecx, dword ptr [eax]
004803f0  mov        eax, dword ptr [eax + 4]
004803f3  mov        dword ptr [ebp - 0x1c], ecx
004803f6  mov        dword ptr [ebp - 0x18], eax

// block 004803f9
004803f9  pop        edi

// block 004803fa
004803fa  cmp        byte ptr [ebp + 0xc], 0
004803fe  je         0x480414

// block 00480400
00480400  mov        ecx, dword ptr [0x4b4310]
00480406  cmp        dword ptr [ecx], 9
00480409  je         0x480414

// block 0048040b
0048040b  lea        eax, [ebp - 0x1c]
0048040e  push       eax
0048040f  call       0x47e32e

// block 00480414
00480414  mov        ecx, dword ptr [ebp - 0x1c]
00480417  mov        eax, dword ptr [ebp - 0x30]
0048041a  mov        dword ptr [eax], ecx
0048041c  mov        ecx, dword ptr [ebp - 0x18]
0048041f  mov        dword ptr [eax + 4], ecx

// block 00480422
00480422  mov        ecx, dword ptr [ebp - 4]
00480425  pop        esi
00480426  xor        ecx, ebp
00480428  pop        ebx
00480429  call       0x46c932

// block 0048042e
0048042e  leave      
0048042f  ret        
