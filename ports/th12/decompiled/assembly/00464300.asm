
// block 00464300
00464300  sub        esp, 0x204
00464306  mov        eax, dword ptr [0x4ad138]
0046430b  xor        eax, esp
0046430d  mov        dword ptr [esp + 0x200], eax
00464314  test       dword ptr [0x4cee78], 0x8000
0046431e  je         0x464331

// block 00464320
00464320  push       0x4cf140
00464325  call       dword ptr [0x498088]

// block 0046432b
0046432b  inc        byte ptr [0x4cf21b]

// block 00464331
00464331  mov        ecx, dword ptr [esp + 0x208]
00464338  lea        eax, [esp + 0x20c]
0046433f  push       eax
00464340  push       ecx
00464341  lea        edx, [esp + 8]
00464345  push       edx
00464346  call       0x46cad8

// block 0046434b
0046434b  lea        eax, [esp + 0xc]
0046434f  add        esp, 0xc
00464352  lea        edx, [eax + 1]

// block 00464355
00464355  mov        cl, byte ptr [eax]
00464357  inc        eax
00464358  test       cl, cl
0046435a  jne        0x464355

// block 0046435c
0046435c  sub        eax, edx
0046435e  push       esi
0046435f  mov        esi, dword ptr [edi + 0x2000]
00464365  lea        ecx, [esi + eax]
00464368  lea        edx, [edi + 0x1fff]
0046436e  cmp        ecx, edx
00464370  jae        0x46438e

// block 00464372
00464372  lea        edx, [esp + 4]

// block 00464376
00464376  mov        cl, byte ptr [edx]
00464378  mov        byte ptr [esi], cl
0046437a  inc        edx
0046437b  inc        esi
0046437c  test       cl, cl
0046437e  jne        0x464376

// block 00464380
00464380  add        dword ptr [edi + 0x2000], eax
00464386  mov        eax, dword ptr [edi + 0x2000]
0046438c  mov        byte ptr [eax], cl

// block 0046438e
0046438e  mov        byte ptr [edi + 0x2004], 1
00464395  test       dword ptr [0x4cee78], 0x8000
0046439f  pop        esi
004643a0  je         0x4643b3

// block 004643a2
004643a2  push       0x4cf140
004643a7  call       dword ptr [0x49808c]

// block 004643ad
004643ad  dec        byte ptr [0x4cf21b]

// block 004643b3
004643b3  mov        ecx, dword ptr [esp + 0x200]
004643ba  mov        eax, dword ptr [esp + 0x208]
004643c1  xor        ecx, esp
004643c3  call       0x46c932

// block 004643c8
004643c8  add        esp, 0x204
004643ce  ret        
