
// block 00468fe0
00468fe0  fldz       
00468fe2  push       esi
00468fe3  fld        dword ptr [esp + 8]
00468fe7  mov        esi, eax
00468fe9  fcom       st(1)
00468feb  fnstsw     ax
00468fed  fstp       st(1)
00468fef  test       ah, 1
00468ff2  jne        0x469007

// block 00468ff4
00468ff4  call       0x4931e0

// block 00468ff9
00468ff9  add        eax, dword ptr [esi + 0x100c]
00468fff  fld        dword ptr [eax + esi + 8]
00469003  pop        esi
00469004  ret        4

// block 00469007
00469007  fld        dword ptr [0x4a3da8]
0046900d  fucomp     st(1)
0046900f  fnstsw     ax
00469011  test       ah, 0x44
00469014  jp         0x469056

// block 00469016
00469016  mov        eax, dword ptr [esi + 0x1008]
0046901c  add        eax, -4
0046901f  js         0x46906d

// block 00469021
00469021  mov        dword ptr [esi + 0x1008], eax
00469027  fstp       st(0)
00469029  mov        ecx, dword ptr [eax + esi + 8]
0046902d  add        eax, -4
00469030  mov        dword ptr [esi + 0x1008], eax
00469036  mov        al, byte ptr [eax + esi + 8]
0046903a  mov        dword ptr [esp + 8], ecx
0046903e  cmp        al, 0x66
00469040  je         0x46904e

// block 00469042
00469042  cmp        al, 0x69
00469044  jne        0x46904e

// block 00469046
00469046  fild       dword ptr [esp + 8]
0046904a  fstp       dword ptr [esp + 8]

// block 0046904e
0046904e  fld        dword ptr [esp + 8]
00469052  pop        esi
00469053  ret        4

// block 00469056
00469056  mov        esi, dword ptr [esi + 0x1014]
0046905c  push       edi
0046905d  mov        edi, dword ptr [esi]
0046905f  call       0x4931e0

// block 00469064
00469064  mov        edx, dword ptr [edi + 0xc]
00469067  push       eax
00469068  mov        ecx, esi
0046906a  call       edx

// block 0046906c
0046906c  pop        edi

// block 0046906d
0046906d  pop        esi
0046906e  ret        4
