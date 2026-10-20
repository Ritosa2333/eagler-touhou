
// block 0040def0
0040def0  sub        esp, 0xc
0040def3  push       ebx
0040def4  mov        ebx, 1
0040def9  test       byte ptr [edi + 0x7c], bl
0040defc  je         0x40df14

// block 0040defe
0040defe  mov        eax, dword ptr [edi + 0x1c]
0040df01  mov        edx, dword ptr [0x4ce8cc]
0040df07  push       eax
0040df08  call       0x461920

// block 0040df0d
0040df0d  test       eax, eax
0040df0f  jne        0x40df1b

// block 0040df11
0040df11  mov        dword ptr [edi + 0x1c], eax

// block 0040df14
0040df14  mov        eax, ebx
0040df16  pop        ebx
0040df17  add        esp, 0xc
0040df1a  ret        

// block 0040df1b
0040df1b  push       esi
0040df1c  mov        esi, dword ptr [0x4b43b8]
0040df22  mov        ecx, 2
0040df27  mov        dword ptr [esi + 0x18f9c], ebx
0040df2d  mov        dword ptr [esi + 0x18f98], ecx
0040df33  mov        dl, byte ptr [eax + 0x3bf]
0040df39  mov        byte ptr [esi + 0x18f83], dl
0040df3f  lea        ebx, [esp + 8]
0040df43  test       byte ptr [edi + 0x7c], cl
0040df46  je         0x40df78

// block 0040df48
0040df48  fld        dword ptr [0x4a4030]
0040df4e  mov        eax, dword ptr [edi + 0x80]
0040df54  fstp       dword ptr [esp + 8]
0040df58  push       eax
0040df59  fld        dword ptr [0x4a402c]
0040df5f  push       0x49f76c
0040df64  fstp       dword ptr [esp + 0x14]
0040df68  fldz       
0040df6a  fstp       dword ptr [esp + 0x18]
0040df6e  call       0x4015c0

// block 0040df73
0040df73  add        esp, 8
0040df76  jmp        0x40df9f

// block 0040df78
0040df78  fld        dword ptr [0x4a4028]
0040df7e  push       0x49f770
0040df83  fstp       dword ptr [esp + 0xc]
0040df87  fld        dword ptr [0x4a402c]
0040df8d  fstp       dword ptr [esp + 0x10]
0040df91  fldz       
0040df93  fstp       dword ptr [esp + 0x14]
0040df97  call       0x4015c0

// block 0040df9c
0040df9c  add        esp, 4

// block 0040df9f
0040df9f  mov        edx, dword ptr [0x4b0c90]
0040dfa5  fld        dword ptr [0x4a4024]
0040dfab  mov        ecx, dword ptr [0x4b0c94]
0040dfb1  fstp       dword ptr [esp + 8]
0040dfb5  mov        eax, dword ptr [edi + 0x78]
0040dfb8  fld        dword ptr [0x4a402c]
0040dfbe  mov        esi, dword ptr [0x4b43b8]
0040dfc4  fstp       dword ptr [esp + 0xc]
0040dfc8  fldz       
0040dfca  lea        ecx, [ecx + edx*2]
0040dfcd  imul       ecx, ecx, 0x45f4
0040dfd3  fstp       dword ptr [esp + 0x10]
0040dfd7  add        ecx, dword ptr [0x4b451c]
0040dfdd  lea        edx, [eax + eax*8]
0040dfe0  shl        edx, 4
0040dfe3  lea        eax, [edx + ecx]
0040dfe6  mov        ecx, dword ptr [eax + 0x6f0]
0040dfec  mov        edx, dword ptr [eax + 0x6ec]
0040dff2  push       ecx
0040dff3  push       edx
0040dff4  push       0x49f774
0040dff9  lea        ebx, [esp + 0x14]
0040dffd  call       0x4015c0

// block 0040e002
0040e002  mov        eax, dword ptr [0x4b43b8]
0040e007  add        esp, 0xc
0040e00a  pop        esi
0040e00b  mov        dword ptr [eax + 0x18f98], 0
0040e015  mov        dword ptr [eax + 0x18f9c], 0
0040e01f  mov        byte ptr [eax + 0x18f83], 0xff
0040e026  mov        eax, 1
0040e02b  pop        ebx
0040e02c  add        esp, 0xc
0040e02f  ret        
