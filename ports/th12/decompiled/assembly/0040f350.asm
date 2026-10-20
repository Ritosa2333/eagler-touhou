
// block 0040f350
0040f350  sub        esp, 0xc
0040f353  fldz       
0040f355  push       ebx
0040f356  fst        dword ptr [esp + 4]
0040f35a  push       esi
0040f35b  mov        esi, dword ptr [0x4b43b8]
0040f361  fst        dword ptr [esp + 0xc]
0040f365  push       0x49f838
0040f36a  fstp       dword ptr [esp + 0x14]
0040f36e  lea        ebx, [esp + 0xc]
0040f372  call       0x401720

// block 0040f377
0040f377  mov        eax, dword ptr [edi + 0x30]
0040f37a  dec        eax
0040f37b  add        esp, 4
0040f37e  cmp        eax, 3
0040f381  ja         0x40f632

// block 0040f387
0040f387  jmp        dword ptr [eax*4 + 0x40f640]

// block 0040f38e
0040f38e  mov        esi, dword ptr [0x4b43b8]
0040f394  fld        dword ptr [0x4a3d78]
0040f39a  fstp       dword ptr [esp + 8]
0040f39e  mov        dword ptr [esi + 0x18f80], 0xffff4040
0040f3a8  mov        eax, dword ptr [edi + 0x114]
0040f3ae  fld        dword ptr [0x4a3e68]
0040f3b4  mov        ecx, dword ptr [edi + 0x34]
0040f3b7  fstp       dword ptr [esp + 0xc]
0040f3bb  mov        edx, dword ptr [ecx + eax*4]
0040f3be  fldz       
0040f3c0  push       edx
0040f3c1  fstp       dword ptr [esp + 0x14]
0040f3c5  push       0x49f844
0040f3ca  lea        ebx, [esp + 0x10]
0040f3ce  call       0x401720

// block 0040f3d3
0040f3d3  mov        eax, dword ptr [0x4b43b8]
0040f3d8  add        esp, 8
0040f3db  pop        esi
0040f3dc  mov        dword ptr [eax + 0x18f80], 0xffffffff
0040f3e6  mov        eax, 1
0040f3eb  pop        ebx
0040f3ec  add        esp, 0xc
0040f3ef  ret        

// block 0040f3f0
0040f3f0  cmp        dword ptr [edi + 0x38], 0
0040f3f4  fld        dword ptr [0x4a3f80]
0040f3fa  mov        esi, dword ptr [0x4b43b8]
0040f400  fstp       dword ptr [esp + 8]
0040f404  fld        dword ptr [0x4a3e68]
0040f40a  lea        ebx, [esp + 8]
0040f40e  fstp       dword ptr [esp + 0xc]
0040f412  fldz       
0040f414  fstp       dword ptr [esp + 0x10]
0040f418  je         0x40f436

// block 0040f41a
0040f41a  mov        ecx, dword ptr [edi + 0x114]
0040f420  mov        edx, dword ptr [edi + 0x34]
0040f423  mov        eax, dword ptr [edx + ecx*4]
0040f426  push       eax
0040f427  push       0x49f850
0040f42c  call       0x401720

// block 0040f431
0040f431  add        esp, 8
0040f434  jmp        0x40f443

// block 0040f436
0040f436  push       0x49f858
0040f43b  call       0x401720

// block 0040f440
0040f440  add        esp, 4

// block 0040f443
0040f443  mov        eax, dword ptr [0x4b43dc]
0040f448  fld        dword ptr [0x4a3f7c]
0040f44e  mov        edx, dword ptr [edi + 0x1ec]
0040f454  fstp       dword ptr [esp + 0xc]
0040f458  mov        esi, dword ptr [0x4b43b8]
0040f45e  lea        ebx, [esp + 8]
0040f462  test       eax, eax
0040f464  je         0x40f47a

// block 0040f466
0040f466  mov        ecx, dword ptr [eax + 0x64]
0040f469  mov        eax, dword ptr [ecx + 0x8c]
0040f46f  mov        ecx, dword ptr [eax + edx*8]
0040f472  push       ecx
0040f473  push       0x49f868
0040f478  jmp        0x40f480

// block 0040f47a
0040f47a  push       edx
0040f47b  push       0x49f870

// block 0040f480
0040f480  call       0x401720

// block 0040f485
0040f485  fld        dword ptr [0x4a3f78]
0040f48b  mov        esi, dword ptr [0x4b43b8]
0040f491  fstp       dword ptr [esp + 0x14]
0040f495  add        esp, 8
0040f498  push       0x49f878
0040f49d  lea        ebx, [esp + 0xc]
0040f4a1  call       0x401720

// block 0040f4a6
0040f4a6  fld        dword ptr [0x4a3d78]
0040f4ac  mov        esi, dword ptr [0x4b43b8]
0040f4b2  fstp       dword ptr [esp + 0xc]
0040f4b6  fild       dword ptr [edi + 0x3c]
0040f4b9  push       0x49f880
0040f4be  fmul       qword ptr [0x4a3e40]
0040f4c4  fadd       qword ptr [0x4a3d68]
0040f4ca  fstp       dword ptr [esp + 0x14]
0040f4ce  call       0x401720

// block 0040f4d3
0040f4d3  mov        eax, dword ptr [0x4b43b8]
0040f4d8  add        esp, 8
0040f4db  mov        dword ptr [eax + 0x18f80], 0xffffffff
0040f4e5  test       byte ptr [edi + 0x78c], 2
0040f4ec  je         0x40f632

// block 0040f4f2
0040f4f2  fld        dword ptr [0x4a3ef0]
0040f4f8  fstp       dword ptr [esp + 8]
0040f4fc  mov        ecx, dword ptr [esp + 8]
0040f500  fld        dword ptr [0x4a3f2c]
0040f506  mov        dword ptr [edi + 0x6ec], ecx
0040f50c  mov        ecx, dword ptr [0x4ce8cc]
0040f512  fstp       dword ptr [esp + 0xc]
0040f516  fldz       
0040f518  mov        edx, dword ptr [esp + 0xc]
0040f51c  fstp       dword ptr [esp + 0x10]
0040f520  mov        dword ptr [edi + 0x6f0], edx
0040f526  mov        eax, dword ptr [esp + 0x10]
0040f52a  mov        dword ptr [edi + 0x6f4], eax
0040f530  push       ecx
0040f531  jmp        0x40f627

// block 0040f536
0040f536  fld        dword ptr [0x4a40d8]
0040f53c  mov        edx, dword ptr [0x4b43dc]
0040f542  mov        eax, dword ptr [edx + 0x70]
0040f545  fstp       dword ptr [esp + 8]
0040f549  fld        dword ptr [0x4a3f2c]
0040f54f  mov        esi, dword ptr [0x4b43b8]
0040f555  fstp       dword ptr [esp + 0xc]
0040f559  push       eax
0040f55a  fldz       
0040f55c  push       0x49f884
0040f561  lea        ebx, [esp + 0x10]
0040f565  fstp       dword ptr [esp + 0x18]
0040f569  call       0x401720

// block 0040f56e
0040f56e  add        esp, 8
0040f571  pop        esi
0040f572  mov        eax, 1
0040f577  pop        ebx
0040f578  add        esp, 0xc
0040f57b  ret        

// block 0040f57c
0040f57c  fld        dword ptr [0x4a3d78]
0040f582  mov        esi, dword ptr [0x4b43b8]
0040f588  fstp       dword ptr [esp + 8]
0040f58c  mov        dword ptr [esi + 0x18f80], 0xffa0a0a0
0040f596  fld        dword ptr [0x4a3e68]
0040f59c  fstp       dword ptr [esp + 0xc]
0040f5a0  fldz       
0040f5a2  fstp       dword ptr [esp + 0x10]
0040f5a6  fld        dword ptr [edi + 0x780]
0040f5ac  call       0x4931e0

// block 0040f5b1
0040f5b1  fld        dword ptr [edi + 0x77c]
0040f5b7  push       eax
0040f5b8  call       0x4931e0

// block 0040f5bd
0040f5bd  push       eax
0040f5be  push       0x49f890
0040f5c3  lea        ebx, [esp + 0x14]
0040f5c7  call       0x401720

// block 0040f5cc
0040f5cc  mov        ecx, dword ptr [0x4b43b8]
0040f5d2  add        esp, 0xc
0040f5d5  mov        dword ptr [ecx + 0x18f80], 0xffffffff
0040f5df  test       byte ptr [edi + 0x78c], 2
0040f5e6  je         0x40f632

// block 0040f5e8
0040f5e8  fld        dword ptr [0x4a3ef0]
0040f5ee  fstp       dword ptr [esp + 8]
0040f5f2  mov        edx, dword ptr [esp + 8]
0040f5f6  fld        dword ptr [0x4a3f2c]
0040f5fc  mov        dword ptr [edi + 0x6ec], edx
0040f602  mov        edx, dword ptr [0x4ce8cc]
0040f608  fstp       dword ptr [esp + 0xc]
0040f60c  fldz       
0040f60e  mov        eax, dword ptr [esp + 0xc]
0040f612  fstp       dword ptr [esp + 0x10]
0040f616  mov        dword ptr [edi + 0x6f0], eax
0040f61c  mov        ecx, dword ptr [esp + 0x10]
0040f620  mov        dword ptr [edi + 0x6f4], ecx
0040f626  push       edx

// block 0040f627
0040f627  lea        eax, [edi + 0x2c8]
0040f62d  call       0x45a9f0

// block 0040f632
0040f632  pop        esi
0040f633  mov        eax, 1
0040f638  pop        ebx
0040f639  add        esp, 0xc
0040f63c  ret        
