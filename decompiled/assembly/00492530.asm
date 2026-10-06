
// block 00492530
00492530  mov        edi, edi
00492532  push       ebp
00492533  mov        ebp, esp
00492535  sub        esp, 0xc
00492538  test       edi, edi
0049253a  jne        0x492546

// block 0049253c
0049253c  call       0x472b94

// block 00492546
00492546  and        dword ptr [ebp - 8], 0
0049254a  cmp        dword ptr [edi], 0
0049254d  mov        byte ptr [ebp - 1], 0
00492551  jle        0x4925a6

// block 00492553
00492553  push       ebx
00492554  push       esi

// block 00492555
00492555  mov        eax, dword ptr [ebp + 8]
00492558  mov        eax, dword ptr [eax + 0x1c]
0049255b  mov        eax, dword ptr [eax + 0xc]
0049255e  mov        ebx, dword ptr [eax]
00492560  lea        esi, [eax + 4]
00492563  test       ebx, ebx
00492565  jle        0x49259a

// block 00492567
00492567  mov        eax, dword ptr [ebp - 8]
0049256a  shl        eax, 4
0049256d  mov        dword ptr [ebp - 0xc], eax

// block 00492570
00492570  mov        ecx, dword ptr [ebp + 8]
00492573  push       dword ptr [ecx + 0x1c]
00492576  mov        eax, dword ptr [esi]
00492578  push       eax
00492579  mov        eax, dword ptr [edi + 4]
0049257c  add        eax, dword ptr [ebp - 0xc]
0049257f  push       eax
00492580  call       0x491fb3

// block 00492585
00492585  add        esp, 0xc
00492588  test       eax, eax
0049258a  jne        0x492596

// block 0049258c
0049258c  dec        ebx
0049258d  add        esi, 4
00492590  test       ebx, ebx
00492592  jg         0x492570

// block 00492594
00492594  jmp        0x49259a

// block 00492596
00492596  mov        byte ptr [ebp - 1], 1

// block 0049259a
0049259a  inc        dword ptr [ebp - 8]
0049259d  mov        eax, dword ptr [ebp - 8]
004925a0  cmp        eax, dword ptr [edi]
004925a2  jl         0x492555

// block 004925a4
004925a4  pop        esi
004925a5  pop        ebx

// block 004925a6
004925a6  mov        al, byte ptr [ebp - 1]
004925a9  leave      
004925aa  ret        
