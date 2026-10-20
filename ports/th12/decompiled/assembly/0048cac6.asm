
// block 0048cac6
0048cac6  mov        edi, edi
0048cac8  push       ebp
0048cac9  mov        ebp, esp
0048cacb  mov        eax, dword ptr [ebp + 8]
0048cace  cmp        eax, -2
0048cad1  jne        0x48caeb

// block 0048cad3
0048cad3  call       0x46e982

// block 0048cad8
0048cad8  and        dword ptr [eax], 0
0048cadb  call       0x46e96f

// block 0048cae0
0048cae0  mov        dword ptr [eax], 9
0048cae6  or         eax, 0xffffffff
0048cae9  pop        ebp
0048caea  ret        

// block 0048caeb
0048caeb  push       esi
0048caec  xor        esi, esi
0048caee  cmp        eax, esi
0048caf0  jl         0x48cb14

// block 0048caf2
0048caf2  cmp        eax, dword ptr [0x4d6308]
0048caf8  jae        0x48cb14

// block 0048cafa
0048cafa  mov        ecx, eax
0048cafc  and        eax, 0x1f
0048caff  sar        ecx, 5
0048cb02  mov        ecx, dword ptr [ecx*4 + 0x4d6320]
0048cb09  shl        eax, 6
0048cb0c  add        eax, ecx
0048cb0e  test       byte ptr [eax + 4], 1
0048cb12  jne        0x48cb38

// block 0048cb14
0048cb14  call       0x46e982

// block 0048cb19
0048cb19  mov        dword ptr [eax], esi
0048cb1b  call       0x46e96f

// block 0048cb20
0048cb20  push       esi
0048cb21  push       esi
0048cb22  push       esi
0048cb23  push       esi
0048cb24  push       esi
0048cb25  mov        dword ptr [eax], 9
0048cb2b  call       0x470f2d

// block 0048cb30
0048cb30  add        esp, 0x14
0048cb33  or         eax, 0xffffffff
0048cb36  jmp        0x48cb3a

// block 0048cb38
0048cb38  mov        eax, dword ptr [eax]

// block 0048cb3a
0048cb3a  pop        esi
0048cb3b  pop        ebp
0048cb3c  ret        
