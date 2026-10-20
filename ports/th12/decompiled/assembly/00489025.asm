
// block 00489025
00489025  mov        edi, edi
00489027  push       ebp
00489028  mov        ebp, esp
0048902a  mov        eax, dword ptr [ebp + 0xc]
0048902d  cdq        
0048902e  and        edx, 0x1f
00489031  sub        esp, 0xc
00489034  add        eax, edx
00489036  mov        edx, dword ptr [ebp + 0xc]
00489039  push       esi
0048903a  sar        eax, 5
0048903d  and        edx, 0x8000001f
00489043  push       edi
00489044  jns        0x48904b

// block 00489046
00489046  dec        edx
00489047  or         edx, 0xffffffe0
0048904a  inc        edx

// block 0048904b
0048904b  and        dword ptr [ebp - 8], 0
0048904f  and        dword ptr [ebp + 0xc], 0
00489053  mov        edi, dword ptr [ebp + 8]
00489056  or         esi, 0xffffffff
00489059  mov        ecx, edx
0048905b  shl        esi, cl
0048905d  mov        dword ptr [ebp - 4], 0x20
00489064  sub        dword ptr [ebp - 4], edx
00489067  push       ebx
00489068  not        esi

// block 0048906a
0048906a  mov        ecx, dword ptr [ebp + 0xc]
0048906d  mov        ebx, dword ptr [edi + ecx*4]
00489070  and        ebx, esi
00489072  mov        dword ptr [ebp - 0xc], ebx
00489075  mov        ebx, dword ptr [edi + ecx*4]
00489078  mov        ecx, edx
0048907a  shr        ebx, cl
0048907c  mov        ecx, dword ptr [ebp + 0xc]
0048907f  or         ebx, dword ptr [ebp - 8]
00489082  mov        dword ptr [edi + ecx*4], ebx
00489085  mov        ebx, dword ptr [ebp - 0xc]
00489088  mov        ecx, dword ptr [ebp - 4]
0048908b  shl        ebx, cl
0048908d  inc        dword ptr [ebp + 0xc]
00489090  cmp        dword ptr [ebp + 0xc], 3
00489094  mov        dword ptr [ebp - 8], ebx
00489097  jl         0x48906a

// block 00489099
00489099  push       2
0048909b  pop        ecx
0048909c  mov        edx, ecx
0048909e  sub        edx, eax
004890a0  lea        edx, [edi + edx*4]
004890a3  pop        ebx

// block 004890a4
004890a4  cmp        ecx, eax
004890a6  jl         0x4890af

// block 004890a8
004890a8  mov        esi, dword ptr [edx]
004890aa  mov        dword ptr [edi + ecx*4], esi
004890ad  jmp        0x4890b3

// block 004890af
004890af  and        dword ptr [edi + ecx*4], 0

// block 004890b3
004890b3  dec        ecx
004890b4  sub        edx, 4
004890b7  test       ecx, ecx
004890b9  jge        0x4890a4

// block 004890bb
004890bb  pop        edi
004890bc  pop        esi
004890bd  leave      
004890be  ret        
