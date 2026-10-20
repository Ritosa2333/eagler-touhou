
// block 004381e0
004381e0  push       ecx
004381e1  push       ebx
004381e2  mov        ebx, 1
004381e7  sub        dword ptr [0x4b0c98], ebx
004381ed  push       ebp
004381ee  push       edi
004381ef  js         0x43820a

// block 004381f1
004381f1  mov        eax, dword ptr [0x4b0c9c]
004381f6  mov        ecx, dword ptr [0x4b0c98]
004381fc  mov        edx, dword ptr [0x4b43e4]
00438202  push       eax
00438203  push       ecx
00438204  push       edx
00438205  call       0x41ce60

// block 0043820a
0043820a  fldz       
0043820c  mov        dword ptr [esi + 0xa28], 2
00438216  mov        eax, dword ptr [esi + 0xa40]
0043821c  xor        ebp, ebp
0043821e  mov        edx, 0xfff0bdc1
00438223  mov        ecx, 0x4b2ed0
00438228  test       bl, al
0043822a  jne        0x43824c

// block 0043822c
0043822c  or         eax, ebx
0043822e  fst        dword ptr [esi + 0xa38]
00438234  mov        dword ptr [esi + 0xa34], ebp
0043823a  mov        dword ptr [esi + 0xa30], edx
00438240  mov        dword ptr [esi + 0xa3c], ecx
00438246  mov        dword ptr [esi + 0xa40], eax

// block 0043824c
0043824c  fst        dword ptr [esi + 0xa38]
00438252  mov        dword ptr [esi + 0xa34], ebp
00438258  mov        dword ptr [esi + 0xa30], 0xffffffff
00438262  mov        eax, dword ptr [esi + 0xc410]
00438268  test       bl, al
0043826a  jne        0x43828e

// block 0043826c
0043826c  or         eax, ebx
0043826e  fstp       dword ptr [esi + 0xc408]
00438274  mov        dword ptr [esi + 0xc404], ebp
0043827a  mov        dword ptr [esi + 0xc400], edx
00438280  mov        dword ptr [esi + 0xc40c], ecx
00438286  mov        dword ptr [esi + 0xc410], eax
0043828c  jmp        0x438290

// block 0043828e
0043828e  fstp       st(0)

// block 00438290
00438290  fld        dword ptr [0x4a4200]
00438296  push       ebp
00438297  lea        eax, [esi + 0x14]
0043829a  fstp       dword ptr [esi + 0xc408]
004382a0  mov        dword ptr [esi + 0xc404], 0xb4
004382aa  mov        dword ptr [esi + 0xc400], 0xb3
004382b4  mov        ecx, dword ptr [esi + 0x10]
004382b7  push       eax
004382b8  call       0x454d10

// block 004382bd
004382bd  lea        edi, [esi + 0x8310]
004382c3  mov        dword ptr [esp + 0xc], 8
004382cb  jmp        0x4382d0

// block 004382d0
004382d0  mov        ecx, dword ptr [edi]
004382d2  push       ecx
004382d3  mov        dword ptr [edi - 0xb0], ebp
004382d9  call       0x461970

// block 004382de
004382de  mov        edx, dword ptr [edi + 4]
004382e1  push       edx
004382e2  call       0x461970

// block 004382e7
004382e7  add        edi, 0xe4
004382ed  sub        dword ptr [esp + 0xc], ebx
004382f1  jne        0x4382d0

// block 004382f3
004382f3  mov        ecx, dword ptr [0x4b43cc]
004382f9  mov        dword ptr [esi + 0xc41c], ebp
004382ff  mov        eax, dword ptr [ecx + 0x7c]
00438302  test       bl, al
00438304  je         0x438328

// block 00438306
00438306  cmp        dword ptr [ecx + 0x28], 0x3c
0043830a  jl         0x438317

// block 0043830c
0043830c  mov        dword ptr [ecx + 0x80], ebp
00438312  and        eax, 0xffffffdd
00438315  jmp        0x438325

// block 00438317
00438317  mov        edx, dword ptr [0x4b43c4]
0043831d  cmp        dword ptr [edx + 0x3c], ebp
00438320  je         0x438328

// block 00438322
00438322  or         eax, 0x20

// block 00438325
00438325  mov        dword ptr [ecx + 0x7c], eax

// block 00438328
00438328  mov        eax, dword ptr [0x4b0ccc]
0043832d  sub        eax, 0x400
00438332  cmp        eax, 0x400
00438337  mov        dword ptr [0x4b0ccc], eax
0043833c  jle        0x43834d

// block 0043833e
0043833e  mov        dword ptr [0x4b0ccc], 0x400
00438348  pop        edi
00438349  pop        ebp
0043834a  pop        ebx
0043834b  pop        ecx
0043834c  ret        

// block 0043834d
0043834d  cmp        eax, 0xfffffc00
00438352  jge        0x43835e

// block 00438354
00438354  mov        dword ptr [0x4b0ccc], 0xfffffc00

// block 0043835e
0043835e  pop        edi
0043835f  pop        ebp
00438360  pop        ebx
00438361  pop        ecx
00438362  ret        
