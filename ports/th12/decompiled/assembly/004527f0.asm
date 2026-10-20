
// block 004527f0
004527f0  push       ecx
004527f1  cmp        dword ptr [0x4ce55c], 0
004527f8  push       edi
004527f9  mov        edi, ecx
004527fb  je         0x452805

// block 004527fd
004527fd  mov        eax, 7
00452802  pop        edi
00452803  pop        ecx
00452804  ret        

// block 00452805
00452805  mov        ecx, dword ptr [0x4b44e8]
0045280b  test       ecx, ecx
0045280d  je         0x452951

// block 00452813
00452813  mov        eax, dword ptr [ecx + 0x60]
00452816  mov        edx, eax
00452818  shr        edx, 2
0045281b  or         edx, eax
0045281d  test       dl, 1
00452820  jne        0x452951

// block 00452826
00452826  test       al, 0x72
00452828  jne        0x452951

// block 0045282e
0045282e  push       esi
0045282f  lea        esi, [edi + 0x30]
00452832  call       0x464a80

// block 00452837
00452837  mov        eax, dword ptr [edi + 0x20]
0045283a  mov        edx, dword ptr [edi + 0x34]
0045283d  cmp        edx, eax
0045283f  mov        dword ptr [esp + 8], eax
00452843  jge        0x45284e

// block 00452845
00452845  fld        dword ptr [edi + 0x38]
00452848  fidiv      dword ptr [esp + 8]
0045284c  jmp        0x4528a2

// block 0045284e
0045284e  mov        ecx, dword ptr [edi + 0x24]
00452851  lea        esi, [ecx + eax]
00452854  cmp        edx, esi
00452856  jge        0x45285c

// block 00452858
00452858  fld1       
0045285a  jmp        0x4528a2

// block 0045285c
0045285c  lea        esi, [ecx + eax]
0045285f  add        esi, dword ptr [edi + 0x28]
00452862  cmp        edx, esi
00452864  jge        0x452948

// block 0045286a
0045286a  mov        edx, dword ptr [edi + 0x28]
0045286d  add        ecx, edx
0045286f  add        ecx, eax
00452871  mov        dword ptr [esp + 8], ecx
00452875  fild       dword ptr [esp + 8]
00452879  test       ecx, ecx
0045287b  jge        0x452883

// block 0045287d
0045287d  fadd       dword ptr [0x4a3e10]

// block 00452883
00452883  fsub       dword ptr [edi + 0x38]
00452886  fstp       dword ptr [esp + 8]
0045288a  fld        dword ptr [esp + 8]
0045288e  mov        dword ptr [esp + 8], edx
00452892  fild       dword ptr [esp + 8]
00452896  test       edx, edx
00452898  jge        0x4528a0

// block 0045289a
0045289a  fadd       dword ptr [0x4a3e10]

// block 004528a0
004528a0  fdivp      st(1)

// block 004528a2
004528a2  fstp       dword ptr [esp + 8]
004528a6  mov        esi, 0x4ce568
004528ab  fild       dword ptr [edi + 0x1c]
004528ae  fmul       dword ptr [esp + 8]
004528b2  fstp       dword ptr [esp + 8]
004528b6  call       0x464440

// block 004528bb
004528bb  xor        edx, edx
004528bd  fld        dword ptr [esp + 8]
004528c1  mov        ecx, 3
004528c6  div        ecx
004528c8  sub        edx, 0
004528cb  je         0x4528e1

// block 004528cd
004528cd  sub        edx, 1
004528d0  je         0x4528d9

// block 004528d2
004528d2  sub        edx, 1
004528d5  jne        0x4528ed

// block 004528d7
004528d7  fchs       

// block 004528d9
004528d9  fstp       dword ptr [0x4cee04]
004528df  jmp        0x4528ef

// block 004528e1
004528e1  fstp       st(0)
004528e3  fldz       
004528e5  fstp       dword ptr [0x4cee04]
004528eb  jmp        0x4528ef

// block 004528ed
004528ed  fstp       st(0)

// block 004528ef
004528ef  mov        esi, 0x4ce568
004528f4  call       0x464440

// block 004528f9
004528f9  xor        edx, edx
004528fb  mov        ecx, 3
00452900  div        ecx
00452902  sub        edx, 0
00452905  je         0x452937

// block 00452907
00452907  sub        edx, 1
0045290a  je         0x452924

// block 0045290c
0045290c  sub        edx, 1
0045290f  jne        0x45293f

// block 00452911
00452911  fld        dword ptr [esp + 8]
00452915  pop        esi
00452916  fchs       
00452918  lea        eax, [ecx - 2]
0045291b  fstp       dword ptr [0x4cee08]
00452921  pop        edi
00452922  pop        ecx
00452923  ret        

// block 00452924
00452924  fld        dword ptr [esp + 8]
00452928  pop        esi
00452929  fstp       dword ptr [0x4cee08]
0045292f  mov        eax, 1
00452934  pop        edi
00452935  pop        ecx
00452936  ret        

// block 00452937
00452937  fldz       
00452939  fstp       dword ptr [0x4cee08]

// block 0045293f
0045293f  pop        esi
00452940  mov        eax, 1
00452945  pop        edi
00452946  pop        ecx
00452947  ret        

// block 00452948
00452948  pop        esi
00452949  mov        eax, 7
0045294e  pop        edi
0045294f  pop        ecx
00452950  ret        

// block 00452951
00452951  mov        eax, 1
00452956  pop        edi
00452957  pop        ecx
00452958  ret        
