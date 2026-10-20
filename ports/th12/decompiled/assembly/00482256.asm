
// block 00482256
00482256  push       0x64
00482258  push       0x4aaef8
0048225d  call       0x46fcec

// block 00482262
00482262  mov        edi, dword ptr [ebp + 0x14]
00482265  xor        esi, esi
00482267  cmp        edi, esi
00482269  jne        0x48226f

// block 0048226b
0048226b  xor        eax, eax
0048226d  jmp        0x4822e1

// block 0048226f
0048226f  push       5
00482271  call       0x46ebd7

// block 00482276
00482276  pop        ecx
00482277  test       eax, eax
00482279  je         0x48226b

// block 0048227b
0048227b  push       5
0048227d  call       0x46ec9a

// block 00482282
00482282  pop        ecx
00482283  mov        dword ptr [ebp - 4], esi
00482286  mov        dword ptr [0x4b42f8], edi
0048228c  mov        eax, dword ptr [ebp + 0x18]
0048228f  mov        dword ptr [0x4b42fc], eax
00482294  mov        dword ptr [0x4b4308], esi
0048229a  mov        dword ptr [0x4b4300], esi
004822a0  mov        dword ptr [0x4b4304], esi
004822a6  push       dword ptr [ebp + 0x20]
004822a9  push       dword ptr [ebp + 0x1c]
004822ac  push       dword ptr [ebp + 0x10]
004822af  push       dword ptr [ebp + 0xc]
004822b2  push       dword ptr [ebp + 8]
004822b5  lea        ecx, [ebp - 0x74]
004822b8  call       0x47e053

// block 004822bd
004822bd  lea        ecx, [ebp - 0x74]
004822c0  call       0x481f3b

// block 004822c5
004822c5  mov        dword ptr [ebp - 0x1c], eax
004822c8  mov        ecx, 0x4b42f8
004822cd  call       0x47d5c0

// block 004822d2
004822d2  mov        dword ptr [ebp - 4], 0xfffffffe
004822d9  call       0x4822e7

// block 004822de
004822de  mov        eax, dword ptr [ebp - 0x1c]

// block 004822e1
004822e1  call       0x46fd31

// block 004822e6
004822e6  ret        
