
// block 004526e0
004526e0  sub        esp, 0xc
004526e3  cmp        dword ptr [0x4ce55c], 0
004526ea  push       edi
004526eb  mov        edi, ecx
004526ed  je         0x4526f9

// block 004526ef
004526ef  mov        eax, 7
004526f4  pop        edi
004526f5  add        esp, 0xc
004526f8  ret        

// block 004526f9
004526f9  push       esi
004526fa  lea        esi, [edi + 0x30]
004526fd  call       0x464a80

// block 00452702
00452702  mov        eax, dword ptr [edi + 0x1c]
00452705  cmp        dword ptr [edi + 0x34], eax
00452708  mov        dword ptr [esp + 0xc], eax
0045270c  jl         0x452719

// block 0045270e
0045270e  pop        esi
0045270f  mov        eax, 7
00452714  pop        edi
00452715  add        esp, 0xc
00452718  ret        

// block 00452719
00452719  mov        eax, dword ptr [edi + 0x20]
0045271c  mov        ecx, dword ptr [edi + 0x24]
0045271f  sub        ecx, eax
00452721  mov        dword ptr [esp + 8], ecx
00452725  fild       dword ptr [esp + 8]
00452729  mov        dword ptr [esp + 0x10], eax
0045272d  mov        esi, 0x4ce568
00452732  fmul       dword ptr [edi + 0x38]
00452735  fstp       dword ptr [esp + 8]
00452739  fld        dword ptr [esp + 8]
0045273d  fidiv      dword ptr [esp + 0xc]
00452741  fstp       dword ptr [esp + 0xc]
00452745  fld        dword ptr [esp + 0xc]
00452749  fiadd      dword ptr [esp + 0x10]
0045274d  fstp       dword ptr [esp + 8]
00452751  call       0x464440

// block 00452756
00452756  xor        edx, edx
00452758  fld        dword ptr [esp + 8]
0045275c  mov        ecx, 3
00452761  div        ecx
00452763  sub        edx, 0
00452766  je         0x45277c

// block 00452768
00452768  sub        edx, 1
0045276b  je         0x452774

// block 0045276d
0045276d  sub        edx, 1
00452770  jne        0x452788

// block 00452772
00452772  fchs       

// block 00452774
00452774  fstp       dword ptr [0x4cee04]
0045277a  jmp        0x45278a

// block 0045277c
0045277c  fstp       st(0)
0045277e  fldz       
00452780  fstp       dword ptr [0x4cee04]
00452786  jmp        0x45278a

// block 00452788
00452788  fstp       st(0)

// block 0045278a
0045278a  mov        esi, 0x4ce568
0045278f  call       0x464440

// block 00452794
00452794  xor        edx, edx
00452796  mov        ecx, 3
0045279b  div        ecx
0045279d  sub        edx, 0
004527a0  je         0x4527d6

// block 004527a2
004527a2  sub        edx, 1
004527a5  je         0x4527c1

// block 004527a7
004527a7  sub        edx, 1
004527aa  jne        0x4527de

// block 004527ac
004527ac  fld        dword ptr [esp + 8]
004527b0  pop        esi
004527b1  fchs       
004527b3  lea        eax, [ecx - 2]
004527b6  fstp       dword ptr [0x4cee08]
004527bc  pop        edi
004527bd  add        esp, 0xc
004527c0  ret        

// block 004527c1
004527c1  fld        dword ptr [esp + 8]
004527c5  pop        esi
004527c6  fstp       dword ptr [0x4cee08]
004527cc  mov        eax, 1
004527d1  pop        edi
004527d2  add        esp, 0xc
004527d5  ret        

// block 004527d6
004527d6  fldz       
004527d8  fstp       dword ptr [0x4cee08]

// block 004527de
004527de  pop        esi
004527df  mov        eax, 1
004527e4  pop        edi
004527e5  add        esp, 0xc
004527e8  ret        
