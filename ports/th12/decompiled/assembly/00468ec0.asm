
// block 00468ec0
00468ec0  push       ecx
00468ec1  push       esi
00468ec2  push       edi
00468ec3  mov        esi, eax
00468ec5  mov        eax, dword ptr [esi + 4]
00468ec8  movzx      edx, word ptr [eax + 8]
00468ecc  mov        edi, 1
00468ed1  shl        edi, cl
00468ed3  test       edi, edx
00468ed5  je         0x468f67

// block 00468edb
00468edb  fldz       
00468edd  lea        ecx, [eax + ecx*4 + 0x10]
00468ee1  fcomp      dword ptr [ecx]
00468ee3  fnstsw     ax
00468ee5  test       ah, 0x41
00468ee8  jp         0x468eff

// block 00468eea
00468eea  fld        dword ptr [ecx]
00468eec  call       0x4931e0

// block 00468ef1
00468ef1  add        eax, dword ptr [esi + 0x100c]
00468ef7  pop        edi
00468ef8  fld        dword ptr [eax + esi + 8]
00468efc  pop        esi
00468efd  pop        ecx
00468efe  ret        

// block 00468eff
00468eff  fld        dword ptr [0x4a3da8]
00468f05  fcomp      dword ptr [ecx]
00468f07  fnstsw     ax
00468f09  test       ah, 0x44
00468f0c  jp         0x468f4c

// block 00468f0e
00468f0e  mov        eax, dword ptr [esi + 0x1008]
00468f14  add        eax, -4
00468f17  js         0x468f44

// block 00468f19
00468f19  mov        dword ptr [esi + 0x1008], eax
00468f1f  mov        ecx, dword ptr [eax + esi + 8]
00468f23  add        eax, -4
00468f26  mov        dword ptr [esi + 0x1008], eax
00468f2c  mov        al, byte ptr [eax + esi + 8]
00468f30  mov        dword ptr [esp + 8], ecx
00468f34  cmp        al, 0x66
00468f36  je         0x468f44

// block 00468f38
00468f38  cmp        al, 0x69
00468f3a  jne        0x468f44

// block 00468f3c
00468f3c  fild       dword ptr [esp + 8]
00468f40  fstp       dword ptr [esp + 8]

// block 00468f44
00468f44  fld        dword ptr [esp + 8]
00468f48  pop        edi
00468f49  pop        esi
00468f4a  pop        ecx
00468f4b  ret        

// block 00468f4c
00468f4c  mov        esi, dword ptr [esi + 0x1014]
00468f52  fld        dword ptr [ecx]
00468f54  mov        edi, dword ptr [esi]
00468f56  call       0x4931e0

// block 00468f5b
00468f5b  mov        edx, dword ptr [edi + 0xc]
00468f5e  push       eax
00468f5f  mov        ecx, esi
00468f61  call       edx

// block 00468f63
00468f63  pop        edi
00468f64  pop        esi
00468f65  pop        ecx
00468f66  ret        

// block 00468f67
00468f67  fld        dword ptr [eax + ecx*4 + 0x10]
00468f6b  pop        edi
00468f6c  pop        esi
00468f6d  pop        ecx
00468f6e  ret        
