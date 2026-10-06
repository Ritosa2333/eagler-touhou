
// block 00474381
00474381  push       0xc
00474383  push       0x4aacc8
00474388  call       0x46fcec

// block 0047438d
0047438d  call       0x475467

// block 00474392
00474392  mov        edi, eax
00474394  push       1
00474396  push       8
00474398  call       0x477813

// block 0047439d
0047439d  pop        ecx
0047439e  pop        ecx
0047439f  mov        esi, eax
004743a1  mov        dword ptr [ebp - 0x1c], esi
004743a4  test       esi, esi
004743a6  jne        0x4743b7

// block 004743a8
004743a8  call       0x46e96f

// block 004743ad
004743ad  mov        dword ptr [eax], 0xc
004743b3  xor        eax, eax
004743b5  jmp        0x474412

// block 004743b7
004743b7  call       0x474181

// block 004743bc
004743bc  call       0x4739a5

// block 004743c1
004743c1  mov        eax, dword ptr [edi + 0x6c]
004743c4  mov        dword ptr [esi], eax
004743c6  mov        eax, dword ptr [edi + 0x68]
004743c9  mov        dword ptr [esi + 4], eax
004743cc  push       0xc
004743ce  call       0x46ec9a

// block 004743d3
004743d3  pop        ecx
004743d4  and        dword ptr [ebp - 4], 0
004743d8  push       dword ptr [esi]
004743da  call       0x473ff5

// block 004743df
004743df  pop        ecx
004743e0  mov        dword ptr [ebp - 4], 0xfffffffe
004743e7  call       0x47441b

// block 004743ec
004743ec  push       0xd
004743ee  call       0x46ec9a

// block 004743f3
004743f3  pop        ecx
004743f4  mov        dword ptr [ebp - 4], 1
004743fb  push       dword ptr [esi + 4]
004743fe  call       dword ptr [0x498160]

// block 00474404
00474404  mov        dword ptr [ebp - 4], 0xfffffffe
0047440b  call       0x474427

// block 00474410
00474410  mov        eax, esi

// block 00474412
00474412  call       0x46fd31

// block 00474417
00474417  ret        
