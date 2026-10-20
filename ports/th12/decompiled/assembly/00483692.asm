
// block 00483692
00483692  mov        edi, edi
00483694  push       ebp
00483695  mov        ebp, esp
00483697  push       ecx
00483698  push       ecx
00483699  mov        eax, dword ptr [0x4ad138]
0048369e  xor        eax, ebp
004836a0  mov        dword ptr [ebp - 4], eax
004836a3  mov        eax, dword ptr [0x4b4398]
004836a8  push       ebx
004836a9  push       esi
004836aa  xor        ebx, ebx
004836ac  push       edi
004836ad  mov        edi, ecx
004836af  cmp        eax, ebx
004836b1  jne        0x4836ed

// block 004836b3
004836b3  lea        eax, [ebp - 8]
004836b6  push       eax
004836b7  xor        esi, esi
004836b9  inc        esi
004836ba  push       esi
004836bb  push       0x49e1c8
004836c0  push       esi
004836c1  call       dword ptr [0x498048]

// block 004836c7
004836c7  test       eax, eax
004836c9  je         0x4836d3

// block 004836cb
004836cb  mov        dword ptr [0x4b4398], esi
004836d1  jmp        0x483707

// block 004836d3
004836d3  call       dword ptr [0x4980e4]

// block 004836d9
004836d9  cmp        eax, 0x78
004836dc  jne        0x4836e8

// block 004836de
004836de  push       2
004836e0  pop        eax
004836e1  mov        dword ptr [0x4b4398], eax
004836e6  jmp        0x4836ed

// block 004836e8
004836e8  mov        eax, dword ptr [0x4b4398]

// block 004836ed
004836ed  cmp        eax, 2
004836f0  je         0x4837c5

// block 004836f6
004836f6  cmp        eax, ebx
004836f8  je         0x4837c5

// block 004836fe
004836fe  cmp        eax, 1
00483701  jne        0x4837ef

// block 00483707
00483707  mov        dword ptr [ebp - 8], ebx
0048370a  cmp        dword ptr [ebp + 0x18], ebx
0048370d  jne        0x483717

// block 0048370f
0048370f  mov        eax, dword ptr [edi]
00483711  mov        eax, dword ptr [eax + 4]
00483714  mov        dword ptr [ebp + 0x18], eax

// block 00483717
00483717  mov        esi, dword ptr [0x4980dc]
0048371d  xor        eax, eax
0048371f  cmp        dword ptr [ebp + 0x20], ebx
00483722  push       ebx
00483723  push       ebx
00483724  push       dword ptr [ebp + 0x10]
00483727  setne      al
0048372a  push       dword ptr [ebp + 0xc]
0048372d  lea        eax, [eax*8 + 1]
00483734  push       eax
00483735  push       dword ptr [ebp + 0x18]
00483738  call       esi

// block 0048373a
0048373a  mov        edi, eax
0048373c  cmp        edi, ebx
0048373e  je         0x4837ef

// block 00483744
00483744  jle        0x483782

// block 00483746
00483746  cmp        edi, 0x7ffffff0
0048374c  ja         0x483782

// block 0048374e
0048374e  lea        eax, [edi + edi + 8]
00483752  cmp        eax, 0x400
00483757  ja         0x48376c

// block 00483759
00483759  call       0x48d830

// block 0048375e
0048375e  mov        eax, esp
00483760  cmp        eax, ebx
00483762  je         0x483780

// block 00483764
00483764  mov        dword ptr [eax], 0xcccc
0048376a  jmp        0x48377d

// block 0048376c
0048376c  push       eax
0048376d  call       0x46d04a

// block 00483772
00483772  pop        ecx
00483773  cmp        eax, ebx
00483775  je         0x483780

// block 00483777
00483777  mov        dword ptr [eax], 0xdddd

// block 0048377d
0048377d  add        eax, 8

// block 00483780
00483780  mov        ebx, eax

// block 00483782
00483782  test       ebx, ebx
00483784  je         0x4837ef

// block 00483786
00483786  lea        eax, [edi + edi]
00483789  push       eax
0048378a  push       0
0048378c  push       ebx
0048378d  call       0x477420

// block 00483792
00483792  add        esp, 0xc
00483795  push       edi
00483796  push       ebx
00483797  push       dword ptr [ebp + 0x10]
0048379a  push       dword ptr [ebp + 0xc]
0048379d  push       1
0048379f  push       dword ptr [ebp + 0x18]
004837a2  call       esi

// block 004837a4
004837a4  test       eax, eax
004837a6  je         0x4837b9

// block 004837a8
004837a8  push       dword ptr [ebp + 0x14]
004837ab  push       eax
004837ac  push       ebx
004837ad  push       dword ptr [ebp + 8]
004837b0  call       dword ptr [0x498048]

// block 004837b6
004837b6  mov        dword ptr [ebp - 8], eax

// block 004837b9
004837b9  push       ebx
004837ba  call       0x48326a

// block 004837bf
004837bf  mov        eax, dword ptr [ebp - 8]
004837c2  pop        ecx
004837c3  jmp        0x48383a

// block 004837c5
004837c5  xor        esi, esi
004837c7  cmp        dword ptr [ebp + 0x1c], ebx
004837ca  jne        0x4837d4

// block 004837cc
004837cc  mov        eax, dword ptr [edi]
004837ce  mov        eax, dword ptr [eax + 0x14]
004837d1  mov        dword ptr [ebp + 0x1c], eax

// block 004837d4
004837d4  cmp        dword ptr [ebp + 0x18], ebx
004837d7  jne        0x4837e1

// block 004837d9
004837d9  mov        eax, dword ptr [edi]
004837db  mov        eax, dword ptr [eax + 4]
004837de  mov        dword ptr [ebp + 0x18], eax

// block 004837e1
004837e1  push       dword ptr [ebp + 0x1c]
004837e4  call       0x48d62f

// block 004837e9
004837e9  pop        ecx
004837ea  cmp        eax, -1
004837ed  jne        0x4837f3

// block 004837ef
004837ef  xor        eax, eax
004837f1  jmp        0x48383a

// block 004837f3
004837f3  cmp        eax, dword ptr [ebp + 0x18]
004837f6  je         0x483816

// block 004837f8
004837f8  push       ebx
004837f9  push       ebx
004837fa  lea        ecx, [ebp + 0x10]
004837fd  push       ecx
004837fe  push       dword ptr [ebp + 0xc]
00483801  push       eax
00483802  push       dword ptr [ebp + 0x18]
00483805  call       0x48d678

// block 0048380a
0048380a  mov        esi, eax
0048380c  add        esp, 0x18
0048380f  cmp        esi, ebx
00483811  je         0x4837ef

// block 00483813
00483813  mov        dword ptr [ebp + 0xc], esi

// block 00483816
00483816  push       dword ptr [ebp + 0x14]
00483819  push       dword ptr [ebp + 0x10]
0048381c  push       dword ptr [ebp + 0xc]
0048381f  push       dword ptr [ebp + 8]
00483822  push       dword ptr [ebp + 0x1c]
00483825  call       dword ptr [0x49804c]

// block 0048382b
0048382b  mov        edi, eax
0048382d  cmp        esi, ebx
0048382f  je         0x483838

// block 00483831
00483831  push       esi
00483832  call       0x46c941

// block 00483837
00483837  pop        ecx

// block 00483838
00483838  mov        eax, edi

// block 0048383a
0048383a  lea        esp, [ebp - 0x14]
0048383d  pop        edi
0048383e  pop        esi
0048383f  pop        ebx
00483840  mov        ecx, dword ptr [ebp - 4]
00483843  xor        ecx, ebp
00483845  call       0x46c932

// block 0048384a
0048384a  leave      
0048384b  ret        
