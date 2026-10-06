
// block 004103f0
004103f0  push       ecx
004103f1  push       ebx
004103f2  push       edi
004103f3  mov        edi, dword ptr [ecx + 0x478]
004103f9  mov        ebx, dword ptr [edi + 0xc8]
004103ff  cmp        ebx, 0x10
00410402  jge        0x41057e

// block 00410408
00410408  mov        eax, ebx
0041040a  cmp        eax, dword ptr [edi + 0xc4]
00410410  je         0x410508

// block 00410416
00410416  test       ebx, ebx
00410418  jle        0x410439

// block 0041041a
0041041a  lea        ecx, [edi + 0x83]
00410420  mov        edx, ebx

// block 00410422
00410422  mov        al, byte ptr [ecx]
00410424  cmp        al, 0x10
00410426  jb         0x41042e

// block 00410428
00410428  sub        al, 0x10
0041042a  mov        byte ptr [ecx], al
0041042c  jmp        0x410431

// block 0041042e
0041042e  mov        byte ptr [ecx], 0

// block 00410431
00410431  add        ecx, 4
00410434  sub        edx, 1
00410437  jne        0x410422

// block 00410439
00410439  push       esi
0041043a  mov        esi, 0x4ce568
0041043f  call       0x464440

// block 00410444
00410444  mov        dword ptr [esp + 0xc], eax
00410448  fild       dword ptr [esp + 0xc]
0041044c  test       eax, eax
0041044e  jge        0x410456

// block 00410450
00410450  fadd       dword ptr [0x4a3e10]

// block 00410456
00410456  fmul       qword ptr [0x4a3eb0]
0041045c  sub        esp, 8
0041045f  lea        edx, [edi + ebx*8]
00410462  mov        ecx, edx
00410464  fstp       dword ptr [esp + 0x14]
00410468  fld        dword ptr [esp + 0x14]
0041046c  fld        qword ptr [0x4a4078]
00410472  fmul       st(1), st(0)
00410474  faddp      st(1)
00410476  fstp       dword ptr [esp + 0x14]
0041047a  fld        dword ptr [esp + 0x14]
0041047e  fstp       dword ptr [esp + 4]
00410482  fld        dword ptr [edi + 0xc0]
00410488  fstp       dword ptr [esp]
0041048b  call       0x4108e0

// block 00410490
00410490  fld        dword ptr [edx - 8]
00410493  mov        esi, 0x4ce568
00410498  fadd       dword ptr [edx]
0041049a  fstp       dword ptr [edx]
0041049c  fld        dword ptr [edx - 4]
0041049f  fadd       dword ptr [edx + 4]
004104a2  fstp       dword ptr [edx + 4]
004104a5  call       0x464440

// block 004104aa
004104aa  mov        dword ptr [esp + 0xc], eax
004104ae  fild       dword ptr [esp + 0xc]
004104b2  test       eax, eax
004104b4  jge        0x4104bc

// block 004104b6
004104b6  fadd       dword ptr [0x4a3e10]

// block 004104bc
004104bc  fmul       qword ptr [0x4a3ea8]
004104c2  sub        esp, 8
004104c5  fsub       qword ptr [0x4a3ce0]
004104cb  fstp       dword ptr [esp + 0x14]
004104cf  fld        dword ptr [esp + 0x14]
004104d3  fmul       qword ptr [0x4a3cd8]
004104d9  fstp       dword ptr [esp + 0x14]
004104dd  fld        dword ptr [esp + 0x14]
004104e1  fdiv       qword ptr [0x4a3e40]
004104e7  fstp       dword ptr [esp + 0x14]
004104eb  fld        dword ptr [esp + 0x14]
004104ef  fstp       dword ptr [esp + 4]
004104f3  fld        dword ptr [edi + 0xc0]
004104f9  fstp       dword ptr [esp]
004104fc  call       0x464640

// block 00410501
00410501  fstp       dword ptr [edi + 0xc0]
00410507  pop        esi

// block 00410508
00410508  mov        edx, dword ptr [edi + 0xc8]
0041050e  mov        ecx, dword ptr [edi + 0xd0]
00410514  mov        dword ptr [edi + 0xc4], edx
0041051a  fld        dword ptr [ecx]
0041051c  fcomp      qword ptr [0x4a3db0]
00410522  fnstsw     ax
00410524  test       ah, 0x41
00410527  jne        0x410557

// block 00410529
00410529  fld        dword ptr [0x4a3dac]
0041052f  fcomp      dword ptr [ecx]
00410531  fnstsw     ax
00410533  test       ah, 0x41
00410536  jne        0x410557

// block 00410538
00410538  fld        dword ptr [edi + 0xcc]
0041053e  inc        edx
0041053f  fadd       qword ptr [0x4a3ce0]
00410545  mov        dword ptr [edi + 0xc8], edx
0041054b  xor        eax, eax
0041054d  fstp       dword ptr [edi + 0xcc]
00410553  pop        edi
00410554  pop        ebx
00410555  pop        ecx
00410556  ret        

// block 00410557
00410557  fld        dword ptr [ecx]
00410559  fadd       dword ptr [edi + 0xcc]
0041055f  fstp       dword ptr [esp + 8]
00410563  fld        dword ptr [esp + 8]
00410567  fst        dword ptr [edi + 0xcc]
0041056d  call       0x4931e0

// block 00410572
00410572  mov        dword ptr [edi + 0xc8], eax
00410578  pop        edi
00410579  xor        eax, eax
0041057b  pop        ebx
0041057c  pop        ecx
0041057d  ret        

// block 0041057e
0041057e  pop        edi
0041057f  mov        eax, 1
00410584  pop        ebx
00410585  pop        ecx
00410586  ret        
