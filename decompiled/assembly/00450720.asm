
// block 00450720
00450720  push       ecx
00450721  call       0x4508b0

// block 00450726
00450726  fst        qword ptr [esi + 0x40]
00450729  cmp        byte ptr [0x4cead3], 1
00450730  jne        0x45077a

// block 00450732
00450732  fld        st(0)
00450734  fsub       qword ptr [esi + 0x60]
00450737  fld        qword ptr [0x4a3de8]
0045073d  fcom       st(1)
0045073f  fnstsw     ax
00450741  test       ah, 0x41
00450744  jne        0x450776

// block 00450746
00450746  fldz       
00450748  fcomp      st(2)
0045074a  fnstsw     ax
0045074c  fstp       st(1)
0045074e  test       ah, 5
00450751  jp         0x450778

// block 00450753
00450753  fadd       qword ptr [esi + 0x60]
00450756  fsubrp     st(1)
00450758  fmul       qword ptr [0x4a3d30]
0045075e  fsub       qword ptr [0x4a4218]
00450764  call       0x4931e0

// block 00450769
00450769  test       eax, eax
0045076b  jle        0x45077c

// block 0045076d
0045076d  push       eax
0045076e  call       dword ptr [0x498084]

// block 00450774
00450774  jmp        0x45077c

// block 00450776
00450776  fstp       st(1)

// block 00450778
00450778  fstp       st(1)

// block 0045077a
0045077a  fstp       st(0)

// block 0045077c
0045077c  call       0x4508b0

// block 00450781
00450781  fstp       qword ptr [esi + 0x68]
00450784  call       0x450810

// block 00450789
00450789  mov        eax, dword ptr [0x4ce8f0]
0045078e  mov        ecx, dword ptr [eax]
00450790  mov        edx, dword ptr [ecx + 0x44]
00450793  push       0
00450795  push       0
00450797  push       0
00450799  push       0
0045079b  push       eax
0045079c  call       edx

// block 0045079e
0045079e  test       eax, eax
004507a0  jge        0x4507dc

// block 004507a2
004507a2  call       0x431700

// block 004507a7
004507a7  call       0x44f370

// block 004507ac
004507ac  mov        eax, dword ptr [0x4ce8f0]
004507b1  mov        ecx, dword ptr [eax]
004507b3  mov        edx, dword ptr [ecx + 0x40]
004507b6  push       0x4ce9dc
004507bb  push       eax
004507bc  call       edx

// block 004507be
004507be  call       0x44f400

// block 004507c3
004507c3  mov        eax, 0x4ce8e8
004507c8  call       0x431630

// block 004507cd
004507cd  call       0x451200

// block 004507d2
004507d2  mov        dword ptr [0x4cee5c], 2

// block 004507dc
004507dc  call       0x4508b0

// block 004507e1
004507e1  fstp       qword ptr [esi + 0x60]
004507e4  cmp        dword ptr [0x4b43e0], 0
004507eb  je         0x4507f2

// block 004507ed
004507ed  call       0x41cb70

// block 004507f2
004507f2  cmp        dword ptr [0x4b43cc], 0
004507f9  je         0x450800

// block 004507fb
004507fb  call       0x40dd50

// block 00450800
00450800  pop        ecx
00450801  ret        
