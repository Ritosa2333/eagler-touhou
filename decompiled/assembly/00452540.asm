
// block 00452540
00452540  sub        esp, 0x10
00452543  fldz       
00452545  push       ebx
00452546  mov        ebx, dword ptr [ecx + 0x18]
00452549  fst        dword ptr [esp + 4]
0045254d  fstp       dword ptr [esp + 8]
00452551  shl        ebx, 0x18
00452554  fld        dword ptr [0x4a3ec4]
0045255a  or         ebx, dword ptr [ecx + 0x20]
0045255d  fstp       dword ptr [esp + 0xc]
00452561  push       edi
00452562  fld        dword ptr [0x4a3ec0]
00452568  lea        edi, [esp + 8]
0045256c  fstp       dword ptr [esp + 0x14]
00452570  call       0x451f30

// block 00452575
00452575  pop        edi
00452576  mov        eax, 1
0045257b  pop        ebx
0045257c  add        esp, 0x10
0045257f  ret        
