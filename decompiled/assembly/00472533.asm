
// block 00472533
00472533  mov        edi, edi
00472535  push       ebp
00472536  mov        ebp, esp
00472538  push       ebx
00472539  xor        ebx, ebx
0047253b  cmp        dword ptr [ebp + 0x10], ebx
0047253e  jne        0x47255d

// block 00472540
00472540  call       0x46e96f

// block 00472545
00472545  push       ebx
00472546  push       ebx
00472547  push       ebx
00472548  push       ebx
00472549  push       ebx
0047254a  mov        dword ptr [eax], 0x16
00472550  call       0x470f2d

// block 00472555
00472555  add        esp, 0x14
00472558  or         eax, 0xffffffff
0047255b  jmp        0x4725b8

// block 0047255d
0047255d  push       esi
0047255e  mov        esi, dword ptr [ebp + 8]
00472561  cmp        esi, ebx
00472563  je         0x47256a

// block 00472565
00472565  cmp        dword ptr [ebp + 0xc], ebx
00472568  ja         0x472577

// block 0047256a
0047256a  call       0x46e96f

// block 0047256f
0047256f  mov        dword ptr [eax], 0x16
00472575  jmp        0x4725a7

// block 00472577
00472577  push       dword ptr [ebp + 0x18]
0047257a  push       dword ptr [ebp + 0x14]
0047257d  push       dword ptr [ebp + 0x10]
00472580  push       dword ptr [ebp + 0xc]
00472583  push       esi
00472584  push       0x47c9cb
00472589  call       0x472414

// block 0047258e
0047258e  add        esp, 0x18
00472591  cmp        eax, ebx
00472593  jge        0x472597

// block 00472595
00472595  mov        byte ptr [esi], bl

// block 00472597
00472597  cmp        eax, -2
0047259a  jne        0x4725b7

// block 0047259c
0047259c  call       0x46e96f

// block 004725a1
004725a1  mov        dword ptr [eax], 0x22

// block 004725a7
004725a7  push       ebx
004725a8  push       ebx
004725a9  push       ebx
004725aa  push       ebx
004725ab  push       ebx
004725ac  call       0x470f2d

// block 004725b1
004725b1  add        esp, 0x14
004725b4  or         eax, 0xffffffff

// block 004725b7
004725b7  pop        esi

// block 004725b8
004725b8  pop        ebx
004725b9  pop        ebp
004725ba  ret        
