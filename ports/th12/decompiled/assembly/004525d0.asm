
// block 004525d0
004525d0  sub        esp, 0xc
004525d3  push       esi
004525d4  xor        edx, edx
004525d6  mov        esi, ecx
004525d8  movzx      ecx, byte ptr [esi + 0x27]
004525dc  cmp        dword ptr [0x4ce55c], edx
004525e2  je         0x4525eb

// block 004525e4
004525e4  xor        eax, eax
004525e6  pop        esi
004525e7  add        esp, 0xc
004525ea  ret        

// block 004525eb
004525eb  mov        eax, dword ptr [esi + 0x1c]
004525ee  cmp        dword ptr [esi + 0x34], eax
004525f1  mov        dword ptr [esp + 4], eax
004525f5  jge        0x452656

// block 004525f7
004525f7  fld        dword ptr [esi + 0x38]
004525fa  mov        eax, ecx
004525fc  mov        dword ptr [esp + 8], eax
00452600  fild       dword ptr [esp + 8]
00452604  push       edi
00452605  test       eax, eax
00452607  jge        0x45260f

// block 00452609
00452609  fadd       dword ptr [0x4a3e10]

// block 0045260f
0045260f  fmulp      st(1)
00452611  fidiv      dword ptr [esp + 8]
00452615  fnstcw     word ptr [esp + 8]
00452619  movzx      eax, word ptr [esp + 8]
0045261e  or         eax, 0xc00
00452623  mov        dword ptr [esp + 0xc], eax
00452627  mov        eax, ecx
00452629  fldcw      word ptr [esp + 0xc]
0045262d  fistp      qword ptr [esp + 0xc]
00452631  mov        edi, dword ptr [esp + 0xc]
00452635  sub        eax, edi
00452637  mov        dword ptr [esi + 0x18], eax
0045263a  fldcw      word ptr [esp + 8]
0045263e  pop        edi
0045263f  jns        0x45266a

// block 00452641
00452641  mov        dword ptr [esi + 0x18], edx
00452644  add        esi, 0x30
00452647  call       0x464a80

// block 0045264c
0045264c  mov        eax, 1
00452651  pop        esi
00452652  add        esp, 0xc
00452655  ret        

// block 00452656
00452656  dec        dword ptr [esi + 0x20]
00452659  cmp        dword ptr [esi + 0x20], edx
0045265c  mov        dword ptr [esi + 0x18], edx
0045265f  jle        0x4525e4

// block 00452661
00452661  push       edx
00452662  lea        eax, [esi + 0x30]
00452665  call       0x4067e0

// block 0045266a
0045266a  add        esi, 0x30
0045266d  call       0x464a80

// block 00452672
00452672  mov        eax, 1
00452677  pop        esi
00452678  add        esp, 0xc
0045267b  ret        
