
// block 004544b0
004544b0  push       ecx
004544b1  push       edi
004544b2  mov        edi, eax
004544b4  mov        eax, dword ptr [esi]
004544b6  test       eax, eax
004544b8  je         0x454575

// block 004544be
004544be  mov        ecx, dword ptr [eax]
004544c0  mov        edx, dword ptr [ecx + 0x48]
004544c3  push       eax
004544c4  call       edx

// block 004544c6
004544c6  mov        eax, dword ptr [esi]
004544c8  mov        ecx, dword ptr [eax]
004544ca  mov        edx, dword ptr [ecx + 0x34]
004544cd  push       0
004544cf  push       eax
004544d0  call       edx

// block 004544d2
004544d2  mov        eax, dword ptr [esi]
004544d4  mov        ecx, dword ptr [eax]
004544d6  mov        edx, dword ptr [ecx + 0x40]
004544d9  push       edi
004544da  push       eax
004544db  call       edx

// block 004544dd
004544dd  mov        dword ptr [esi + 0x10], edi
004544e0  cmp        dword ptr [0x4d4780], 0
004544e7  je         0x454551

// block 004544e9
004544e9  fild       dword ptr [0x4d4780]
004544ef  mov        eax, dword ptr [esi + 8]
004544f2  movsx      ecx, word ptr [eax + 8]
004544f6  mov        edi, dword ptr [esi]
004544f8  fdiv       qword ptr [0x4a3ce8]
004544fe  add        ecx, 0x1388
00454504  push       ebx
00454505  mov        ebx, dword ptr [edi]
00454507  fstp       dword ptr [esp + 8]
0045450b  fld        dword ptr [esp + 8]
0045450f  fld1       
00454511  fld        st(0)
00454513  fsubrp     st(2)
00454515  fxch       st(1)
00454517  fstp       dword ptr [esp + 8]
0045451b  fld        dword ptr [esp + 8]
0045451f  fld        st(0)
00454521  fmul       st(0), st(0)
00454523  fmulp      st(1)
00454525  fstp       dword ptr [esp + 8]
00454529  fsub       dword ptr [esp + 8]
0045452d  fstp       dword ptr [esp + 8]
00454531  fld        dword ptr [esp + 8]
00454535  mov        dword ptr [esp + 8], ecx
00454539  fimul      dword ptr [esp + 8]
0045453d  call       0x4931e0

// block 00454542
00454542  mov        edx, dword ptr [ebx + 0x3c]
00454545  sub        eax, 0x1388
0045454a  push       eax
0045454b  push       edi
0045454c  call       edx

// block 0045454e
0045454e  pop        ebx
0045454f  jmp        0x454560

// block 00454551
00454551  mov        eax, dword ptr [esi]
00454553  mov        ecx, dword ptr [eax]
00454555  mov        edx, dword ptr [ecx + 0x3c]
00454558  push       0xffffd8f0
0045455d  push       eax
0045455e  call       edx

// block 00454560
00454560  mov        edx, dword ptr [esi + 8]
00454563  mov        edx, dword ptr [edx + 0xc]
00454566  mov        eax, dword ptr [esi]
00454568  mov        ecx, dword ptr [eax]
0045456a  push       edx
0045456b  push       0
0045456d  push       0
0045456f  push       eax
00454570  mov        eax, dword ptr [ecx + 0x30]
00454573  call       eax

// block 00454575
00454575  pop        edi
00454576  pop        ecx
00454577  ret        
