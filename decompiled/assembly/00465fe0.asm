
// block 00465fe0
00465fe0  sub        esp, 0xc
00465fe3  push       ebp
00465fe4  mov        ebp, dword ptr [esp + 0x14]
00465fe8  push       edi
00465fe9  xor        edi, edi
00465feb  mov        dword ptr [esp + 8], edi
00465fef  mov        dword ptr [esp + 0x18], edi
00465ff3  mov        dword ptr [esp + 0xc], edi
00465ff7  cmp        ebp, edi
00465ff9  jne        0x466008

// block 00465ffb
00465ffb  pop        edi
00465ffc  mov        eax, 0x800401f0
00466001  pop        ebp
00466002  add        esp, 0xc
00466005  ret        8

// block 00466008
00466008  mov        eax, dword ptr [ebp]
0046600b  mov        edx, dword ptr [eax + 0x24]
0046600e  lea        ecx, [esp + 0x10]
00466012  push       ecx
00466013  push       ebp
00466014  call       edx

// block 00466016
00466016  cmp        eax, edi
00466018  jl         0x466212

// block 0046601e
0046601e  test       byte ptr [esp + 0x10], 2
00466023  push       esi
00466024  je         0x466051

// block 00466026
00466026  mov        esi, dword ptr [0x498084]
0046602c  lea        esp, [esp]

// block 00466030
00466030  mov        eax, dword ptr [ebp]
00466033  mov        ecx, dword ptr [eax + 0x50]
00466036  push       ebp
00466037  call       ecx

// block 00466039
00466039  cmp        eax, 0x88780096
0046603e  jne        0x466044

// block 00466040
00466040  push       0xa
00466042  call       esi

// block 00466044
00466044  mov        edx, dword ptr [ebp]
00466047  mov        eax, dword ptr [edx + 0x50]
0046604a  push       ebp
0046604b  call       eax

// block 0046604d
0046604d  test       eax, eax
0046604f  jne        0x466030

// block 00466051
00466051  mov        ecx, dword ptr [ebp]
00466054  push       edi
00466055  push       edi
00466056  push       edi
00466057  lea        edx, [esp + 0x28]
0046605b  push       edx
0046605c  mov        edx, dword ptr [ebx + 8]
0046605f  lea        eax, [esp + 0x1c]
00466063  push       eax
00466064  mov        eax, dword ptr [ecx + 0x2c]
00466067  push       edx
00466068  push       edi
00466069  push       ebp
0046606a  call       eax

// block 0046606c
0046606c  cmp        eax, edi
0046606e  jl         0x466211

// block 00466074
00466074  mov        esi, dword ptr [ebx + 0xc]
00466077  cmp        dword ptr [esi + 0x7c], edi
0046607a  je         0x46609d

// block 0046607c
0046607c  mov        ecx, dword ptr [esi + 0x80]
00466082  mov        edx, dword ptr [esi + 0x90]
00466088  mov        dword ptr [esi + 0x84], ecx
0046608e  mov        eax, dword ptr [edx + 0x1c]
00466091  cmp        eax, edi
00466093  jle        0x4660cc

// block 00466095
00466095  mov        dword ptr [esi + 0x88], eax
0046609b  jmp        0x4660cc

// block 0046609d
0046609d  mov        eax, dword ptr [esi + 0x8c]
004660a3  cmp        eax, edi
004660a5  je         0x4660cc

// block 004660a7
004660a7  mov        ecx, dword ptr [esi + 0x90]
004660ad  mov        edx, dword ptr [ecx + 0x10]
004660b0  add        edx, dword ptr [0x4d4760]
004660b6  push       edi
004660b7  push       edi
004660b8  push       edx
004660b9  push       eax
004660ba  call       dword ptr [0x4980a0]

// block 004660c0
004660c0  mov        eax, dword ptr [esi + 0x90]
004660c6  mov        ecx, dword ptr [eax + 0x1c]
004660c9  mov        dword ptr [esi + 8], ecx

// block 004660cc
004660cc  mov        eax, dword ptr [esp + 0xc]
004660d0  mov        esi, dword ptr [ebx + 0xc]
004660d3  lea        edx, [esp + 0x10]
004660d7  push       edx
004660d8  push       eax
004660d9  mov        eax, dword ptr [esp + 0x24]
004660dd  call       0x466d70

// block 004660e2
004660e2  cmp        eax, edi
004660e4  jl         0x466211

// block 004660ea
004660ea  mov        edi, dword ptr [esp + 0x10]
004660ee  test       edi, edi
004660f0  jne        0x46611c

// block 004660f2
004660f2  mov        ecx, dword ptr [esp + 0x1c]
004660f6  mov        edx, dword ptr [ebx + 0xc]
004660f9  mov        eax, dword ptr [edx + 0x90]
004660ff  mov        edx, dword ptr [esp + 0xc]
00466103  push       ecx
00466104  xor        ecx, ecx
00466106  cmp        word ptr [eax + 0x2e], 8
0046610b  setne      cl
0046610e  dec        ecx
0046610f  and        ecx, 0x80
00466115  push       ecx
00466116  push       edx
00466117  jmp        0x4661f0

// block 0046611c
0046611c  mov        eax, dword ptr [esp + 0x1c]
00466120  cmp        edi, eax
00466122  jae        0x4661f8

// block 00466128
00466128  cmp        dword ptr [esp + 0x20], 0
0046612d  je         0x4661cb

// block 00466133
00466133  mov        esi, dword ptr [ebx + 0xc]
00466136  cmp        dword ptr [esi + 0x7c], 0
0046613a  je         0x46615d

// block 0046613c
0046613c  mov        eax, dword ptr [esi + 0x80]
00466142  mov        ecx, dword ptr [esi + 0x90]
00466148  mov        dword ptr [esi + 0x84], eax
0046614e  mov        eax, dword ptr [ecx + 0x1c]
00466151  test       eax, eax
00466153  jle        0x46618e

// block 00466155
00466155  mov        dword ptr [esi + 0x88], eax
0046615b  jmp        0x46618e

// block 0046615d
0046615d  mov        eax, dword ptr [esi + 0x8c]
00466163  test       eax, eax
00466165  je         0x4661bd

// block 00466167
00466167  mov        edx, dword ptr [esi + 0x90]
0046616d  mov        ecx, dword ptr [edx + 0x10]
00466170  add        ecx, dword ptr [0x4d4760]
00466176  push       0
00466178  push       0
0046617a  push       ecx
0046617b  push       eax
0046617c  call       dword ptr [0x4980a0]

// block 00466182
00466182  mov        edx, dword ptr [esi + 0x90]
00466188  mov        eax, dword ptr [edx + 0x1c]
0046618b  mov        dword ptr [esi + 8], eax

// block 0046618e
0046618e  mov        edx, dword ptr [esp + 0xc]
00466192  mov        eax, dword ptr [esp + 0x1c]
00466196  mov        esi, dword ptr [ebx + 0xc]
00466199  lea        ecx, [esp + 0x10]
0046619d  push       ecx
0046619e  lea        ecx, [edi + edx]
004661a1  sub        eax, edi
004661a3  push       ecx
004661a4  call       0x466d70

// block 004661a9
004661a9  test       eax, eax
004661ab  jl         0x466211

// block 004661ad
004661ad  add        edi, dword ptr [esp + 0x10]
004661b1  cmp        edi, dword ptr [esp + 0x1c]
004661b5  jb         0x466133

// block 004661bb
004661bb  jmp        0x4661f8

// block 004661bd
004661bd  pop        esi
004661be  pop        edi
004661bf  mov        eax, 0x800401f0
004661c4  pop        ebp
004661c5  add        esp, 0xc
004661c8  ret        8

// block 004661cb
004661cb  mov        edx, dword ptr [ebx + 0xc]
004661ce  sub        eax, edi
004661d0  push       eax
004661d1  mov        eax, dword ptr [edx + 0x90]
004661d7  mov        edx, dword ptr [esp + 0x10]
004661db  xor        ecx, ecx
004661dd  cmp        word ptr [eax + 0x2e], 8
004661e2  setne      cl
004661e5  dec        ecx
004661e6  and        ecx, 0x80
004661ec  push       ecx
004661ed  add        edi, edx
004661ef  push       edi

// block 004661f0
004661f0  call       0x477420

// block 004661f5
004661f5  add        esp, 0xc

// block 004661f8
004661f8  mov        ecx, dword ptr [esp + 0x1c]
004661fc  mov        edx, dword ptr [esp + 0xc]
00466200  mov        eax, dword ptr [ebp]
00466203  mov        eax, dword ptr [eax + 0x4c]
00466206  push       0
00466208  push       0
0046620a  push       ecx
0046620b  push       edx
0046620c  push       ebp
0046620d  call       eax

// block 0046620f
0046620f  xor        eax, eax

// block 00466211
00466211  pop        esi

// block 00466212
00466212  pop        edi
00466213  pop        ebp
00466214  add        esp, 0xc
00466217  ret        8
