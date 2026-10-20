
// block 00410170
00410170  mov        eax, dword ptr [edi]
00410172  test       eax, eax
00410174  je         0x4101eb

// block 00410176
00410176  mov        ecx, dword ptr [edi + 0x10]
00410179  mov        edx, dword ptr [edi + 0x14]
0041017c  push       ebx
0041017d  xor        ebx, ebx
0041017f  test       eax, eax
00410181  jle        0x4101ea

// block 00410183
00410183  fld        qword ptr [0x4a3d18]
00410189  push       esi
0041018a  fld        qword ptr [0x4a3d10]
00410190  fldz       

// block 00410192
00410192  xor        esi, esi
00410194  cmp        dword ptr [edi + 4], esi
00410197  jle        0x4101de

// block 00410199
00410199  mov        eax, dword ptr [edx]
0041019b  mov        dword ptr [ecx], eax
0041019d  mov        eax, dword ptr [edx + 4]
004101a0  mov        dword ptr [ecx + 4], eax
004101a3  mov        eax, dword ptr [edx + 8]
004101a6  mov        dword ptr [ecx + 8], eax
004101a9  fld        dword ptr [edx]
004101ab  fdiv       st(3)
004101ad  fstp       dword ptr [ecx + 0x14]
004101b0  fld        dword ptr [edx + 4]
004101b3  fdiv       st(2)
004101b5  fstp       dword ptr [ecx + 0x18]
004101b8  fcom       dword ptr [ecx + 0x14]
004101bb  fnstsw     ax
004101bd  test       ah, 0x41
004101c0  jne        0x4101c5

// block 004101c2
004101c2  fst        dword ptr [ecx + 0x14]

// block 004101c5
004101c5  fcom       dword ptr [ecx + 0x18]
004101c8  fnstsw     ax
004101ca  test       ah, 0x41
004101cd  jne        0x4101d2

// block 004101cf
004101cf  fst        dword ptr [ecx + 0x18]

// block 004101d2
004101d2  inc        esi
004101d3  add        edx, 0xc
004101d6  add        ecx, 0x1c
004101d9  cmp        esi, dword ptr [edi + 4]
004101dc  jl         0x410199

// block 004101de
004101de  inc        ebx
004101df  cmp        ebx, dword ptr [edi]
004101e1  jl         0x410192

// block 004101e3
004101e3  fstp       st(0)
004101e5  pop        esi
004101e6  fstp       st(1)
004101e8  fstp       st(0)

// block 004101ea
004101ea  pop        ebx

// block 004101eb
004101eb  ret        
