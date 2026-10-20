
// block 00453d90
00453d90  push       esi
00453d91  lea        eax, [edx + edx*4]
00453d94  push       edi
00453d95  movsx      edi, word ptr [eax*4 + 0x4ae5aa]
00453d9d  xor        eax, eax
00453d9f  mov        ecx, 0x4cf508

// block 00453da4
00453da4  mov        esi, dword ptr [ecx]
00453da6  test       esi, esi
00453da8  jl         0x453de8

// block 00453daa
00453daa  cmp        esi, edx
00453dac  je         0x453dbd

// block 00453dae
00453dae  add        ecx, 4
00453db1  inc        eax
00453db2  cmp        ecx, 0x4cf538
00453db8  jl         0x453da4

// block 00453dba
00453dba  pop        edi
00453dbb  pop        esi
00453dbc  ret        

// block 00453dbd
00453dbd  mov        ecx, dword ptr [eax*4 + 0x4cf538]
00453dc4  cmp        ecx, 0x80
00453dca  jge        0x453e14

// block 00453dcc
00453dcc  mov        edx, eax
00453dce  shl        edx, 7
00453dd1  add        edx, ecx
00453dd3  pop        edi
00453dd4  mov        dword ptr [edx*4 + 0x4cf568], 0
00453ddf  inc        dword ptr [eax*4 + 0x4cf538]
00453de6  pop        esi
00453de7  ret        

// block 00453de8
00453de8  cmp        eax, 0xc
00453deb  jge        0x453e14

// block 00453ded
00453ded  mov        ecx, eax
00453def  mov        dword ptr [eax*4 + 0x4cf508], edx
00453df6  shl        ecx, 9
00453df9  mov        dword ptr [ecx + 0x4cf568], 0
00453e03  inc        dword ptr [eax*4 + 0x4cf538]
00453e0a  lea        edx, [edx + edx*2]
00453e0d  mov        dword ptr [edx*8 + 0x4d0e74], edi

// block 00453e14
00453e14  pop        edi
00453e15  pop        esi
00453e16  ret        
