
// block 004314d0
004314d0  mov        eax, dword ptr [0x4cee70]
004314d5  push       esi
004314d6  push       0
004314d8  push       0x51
004314da  push       ebx
004314db  push       eax
004314dc  mov        eax, 0x1c
004314e1  xor        ecx, ecx
004314e3  call       0x4615a0

// block 004314e8
004314e8  mov        ecx, dword ptr [ebx]
004314ea  mov        edx, dword ptr [0x4ce8cc]
004314f0  push       ecx
004314f1  call       0x461920

// block 004314f6
004314f6  mov        esi, eax
004314f8  test       esi, esi
004314fa  jne        0x4314fe

// block 004314fc
004314fc  mov        dword ptr [ebx], eax

// block 004314fe
004314fe  lea        edx, [edi*8]
00431505  sub        edx, edi
00431507  add        edx, edx
00431509  add        edx, edx
0043150b  add        edx, edx
0043150d  push       edx
0043150e  call       0x46d04a

// block 00431513
00431513  add        esp, 4
00431516  cmp        edi, 2
00431519  mov        dword ptr [esi + 0x478], eax
0043151f  jle        0x43156d

// block 00431521
00431521  mov        ecx, dword ptr [esi + 0x47c]
00431527  and        ecx, 0xf67fffff
0043152d  or         ecx, 0x6000000
00431533  mov        dword ptr [esi + 0x47c], ecx
00431539  lea        ecx, [edi + edi]
0043153c  mov        dword ptr [esi + 0x3fc], edi
00431542  test       ecx, ecx
00431544  jle        0x431577

// block 00431546
00431546  fldz       
00431548  add        eax, 0xc
0043154b  fld1       
0043154d  or         edx, 0xffffffff

// block 00431550
00431550  fxch       st(1)
00431552  mov        dword ptr [eax + 4], edx
00431555  fst        dword ptr [eax - 4]
00431558  add        eax, 0x1c
0043155b  sub        ecx, 1
0043155e  fxch       st(1)
00431560  fst        dword ptr [eax - 0x1c]
00431563  jne        0x431550

// block 00431565
00431565  fstp       st(1)
00431567  mov        eax, ebx
00431569  fstp       st(0)
0043156b  pop        esi
0043156c  ret        

// block 0043156d
0043156d  and        dword ptr [esi + 0x47c], 0xf07fffff

// block 00431577
00431577  mov        eax, ebx
00431579  pop        esi
0043157a  ret        
