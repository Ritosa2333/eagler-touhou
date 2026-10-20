
// block 004107e0
004107e0  push       ecx
004107e1  push       edi
004107e2  mov        edi, dword ptr [ecx + 0x478]
004107e8  cmp        dword ptr [edi + 0x10], 0x32
004107ec  jge        0x41082f

// block 004107ee
004107ee  mov        eax, dword ptr [edi + 0x10]
004107f1  cmp        eax, dword ptr [edi + 0xc]
004107f4  push       esi
004107f5  lea        esi, [edi + 0xc]
004107f8  je         0x410814

// block 004107fa
004107fa  push       edi
004107fb  push       1
004107fd  lea        ecx, [esp + 0x10]
00410801  push       ecx
00410802  call       0x40fbe0

// block 00410807
00410807  push       edi
00410808  push       1
0041080a  lea        edx, [esp + 0x10]
0041080e  push       edx
0041080f  call       0x40fbe0

// block 00410814
00410814  fld        dword ptr [edi + 0x20]
00410817  add        byte ptr [edi + 0x2b], 3
0041081b  fsub       qword ptr [0x4a4220]
00410821  fstp       dword ptr [edi + 0x20]
00410824  call       0x464a80

// block 00410829
00410829  pop        esi
0041082a  xor        eax, eax
0041082c  pop        edi
0041082d  pop        ecx
0041082e  ret        

// block 0041082f
0041082f  mov        eax, 1
00410834  pop        edi
00410835  pop        ecx
00410836  ret        
