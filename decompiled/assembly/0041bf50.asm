
// block 0041bf50
0041bf50  push       ebp
0041bf51  mov        ebp, esp
0041bf53  and        esp, 0xfffffff8
0041bf56  mov        eax, dword ptr [0x4b43c8]
0041bf5b  sub        esp, 8
0041bf5e  push       esi
0041bf5f  mov        esi, ecx
0041bf61  add        eax, 0x64
0041bf64  push       edi
0041bf65  xor        ecx, ecx
0041bf67  mov        edx, 1
0041bf6c  lea        esp, [esp]

// block 0041bf70
0041bf70  test       byte ptr [eax], dl
0041bf72  je         0x41bf8b

// block 0041bf74
0041bf74  cmp        word ptr [eax + 0x532], dx
0041bf7b  jne        0x41bf8b

// block 0041bf7d
0041bf7d  mov        edi, dword ptr [esi + 0x244]
0041bf83  cmp        edi, dword ptr [eax + 0x50c]
0041bf89  je         0x41bfac

// block 0041bf8b
0041bf8b  add        ecx, edx
0041bf8d  add        eax, 0x9f8
0041bf92  cmp        ecx, 0x7d0
0041bf98  jl         0x41bf70

// block 0041bf9a
0041bf9a  xor        eax, eax
0041bf9c  cmp        dword ptr [esi + 0x270], 0x14
0041bfa3  setge      al
0041bfa6  pop        edi
0041bfa7  pop        esi
0041bfa8  mov        esp, ebp
0041bfaa  pop        ebp
0041bfab  ret        

// block 0041bfac
0041bfac  fld        dword ptr [esi + 0x38]
0041bfaf  fsub       dword ptr [eax + 0x4c0]
0041bfb5  fld        dword ptr [esi + 0x34]
0041bfb8  fsub       dword ptr [eax + 0x4bc]
0041bfbe  fmul       st(0), st(0)
0041bfc0  fld        st(1)
0041bfc2  fmulp      st(2)
0041bfc4  faddp      st(1)
0041bfc6  fstp       dword ptr [esp + 0xc]
0041bfca  fld        dword ptr [esp + 0xc]
0041bfce  call       0x4937c0

// block 0041bfd3
0041bfd3  fstp       dword ptr [esp + 0xc]
0041bfd7  mov        eax, dword ptr [esi + 0xe0]
0041bfdd  fld        dword ptr [esp + 0xc]
0041bfe1  mov        edx, dword ptr [0x4ce8cc]
0041bfe7  fstp       dword ptr [esp + 0xc]
0041bfeb  push       eax
0041bfec  call       0x461920

// block 0041bff1
0041bff1  test       eax, eax
0041bff3  jne        0x41bffb

// block 0041bff5
0041bff5  mov        dword ptr [esi + 0xe0], eax

// block 0041bffb
0041bffb  fld        dword ptr [esp + 0xc]
0041bfff  or         dword ptr [eax + 0x47c], 8
0041c006  pop        edi
0041c007  fstp       dword ptr [eax + 0x58]
0041c00a  xor        eax, eax
0041c00c  pop        esi
0041c00d  mov        esp, ebp
0041c00f  pop        ebp
0041c010  ret        
