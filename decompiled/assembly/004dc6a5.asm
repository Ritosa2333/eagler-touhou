
// block 004dc6a5
004dc6a5  sub        esp, 0x98
004dc6ab  push       ebx
004dc6ac  push       ebp
004dc6ad  push       esi
004dc6ae  mov        edx, ecx
004dc6b0  push       edi
004dc6b1  mov        ecx, 0xf
004dc6b6  mov        ebp, dword ptr [edx + 0x84]
004dc6bc  xor        eax, eax
004dc6be  lea        edi, [esp + 0x2c]
004dc6c2  xor        esi, esi

// block 004dc6c4
004dc6c4  rep stosd  dword ptr es:[edi], eax

// block 004dc6c6
004dc6c6  mov        edi, dword ptr [esp + 0xac]
004dc6cd  cmp        ebp, esi
004dc6cf  mov        dword ptr [esp + 0x20], edx
004dc6d3  jbe        0x4dc6ea

// block 004dc6d5
004dc6d5  xor        ecx, ecx
004dc6d7  mov        cl, byte ptr [eax + edi]
004dc6da  mov        ebx, dword ptr [esp + ecx*4 + 0x28]
004dc6de  lea        ecx, [esp + ecx*4 + 0x28]
004dc6e2  inc        ebx
004dc6e3  inc        eax
004dc6e4  cmp        eax, ebp
004dc6e6  mov        dword ptr [ecx], ebx
004dc6e8  jb         0x4dc6d5

// block 004dc6ea
004dc6ea  mov        ecx, 0x17
004dc6ef  mov        dword ptr [esp + 0x28], esi
004dc6f3  mov        dword ptr [edx + 4], esi
004dc6f6  mov        dword ptr [edx + 0x44], esi
004dc6f9  mov        dword ptr [esp + 0x68], esi
004dc6fd  xor        edi, edi
004dc6ff  mov        dword ptr [esp + 0x1c], esi
004dc703  mov        dword ptr [esp + 0x10], 1
004dc70b  mov        dword ptr [esp + 0x18], ecx
004dc70f  lea        ebp, [edx + 8]
004dc712  mov        dword ptr [esp + 0x14], esi

// block 004dc716
004dc716  mov        eax, dword ptr [esp + esi + 0x2c]
004dc71a  shl        eax, cl
004dc71c  add        edi, eax
004dc71e  cmp        edi, 0x1000000
004dc724  mov        dword ptr [esp + 0x24], edi
004dc728  ja         0x4dc7bc

// block 004dc72e
004dc72e  mov        eax, dword ptr [esp + esi + 0x28]
004dc732  mov        dword ptr [ebp], edi
004dc735  mov        ebx, dword ptr [ebp + 0x3c]
004dc738  add        eax, ebx
004dc73a  cmp        ecx, 0x10
004dc73d  mov        dword ptr [ebp + 0x40], eax
004dc740  mov        dword ptr [esp + esi + 0x6c], eax
004dc744  jl         0x4dc793

// block 004dc746
004dc746  mov        esi, dword ptr [ebp]
004dc749  mov        eax, dword ptr [esp + 0x10]
004dc74d  mov        ebx, dword ptr [esp + 0x1c]
004dc751  mov        edi, dword ptr [edx + 0x8c]
004dc757  shr        esi, 0x10
004dc75a  mov        ecx, esi
004dc75c  and        eax, 0xff
004dc761  sub        ecx, ebx
004dc763  add        edi, ebx
004dc765  mov        bl, al
004dc767  mov        edx, ecx
004dc769  mov        bh, bl
004dc76b  mov        dword ptr [esp + 0x1c], esi
004dc76f  mov        eax, ebx
004dc771  mov        esi, dword ptr [esp + 0x14]
004dc775  shl        eax, 0x10
004dc778  mov        ax, bx
004dc77b  shr        ecx, 2

// block 004dc77e
004dc77e  rep stosd  dword ptr es:[edi], eax

// block 004dc780
004dc780  mov        ecx, edx
004dc782  mov        edx, dword ptr [esp + 0x20]
004dc786  and        ecx, 3

// block 004dc789
004dc789  rep stosb  byte ptr es:[edi], al

// block 004dc78b
004dc78b  mov        edi, dword ptr [esp + 0x24]
004dc78f  mov        ecx, dword ptr [esp + 0x18]

// block 004dc793
004dc793  mov        eax, dword ptr [esp + 0x10]
004dc797  add        esi, 4
004dc79a  inc        eax
004dc79b  dec        ecx
004dc79c  add        ebp, 4
004dc79f  cmp        ecx, 9
004dc7a2  mov        dword ptr [esp + 0x10], eax
004dc7a6  mov        dword ptr [esp + 0x18], ecx
004dc7aa  mov        dword ptr [esp + 0x14], esi
004dc7ae  jge        0x4dc716

// block 004dc7b4
004dc7b4  cmp        edi, 0x1000000
004dc7ba  je         0x4dc7cb

// block 004dc7bc
004dc7bc  pop        edi
004dc7bd  pop        esi
004dc7be  pop        ebp
004dc7bf  xor        al, al
004dc7c1  pop        ebx
004dc7c2  add        esp, 0x98
004dc7c8  ret        4

// block 004dc7cb
004dc7cb  mov        eax, dword ptr [edx + 0x84]
004dc7d1  xor        ecx, ecx
004dc7d3  test       eax, eax
004dc7d5  jbe        0x4dc812

// block 004dc7d7
004dc7d7  mov        esi, dword ptr [esp + 0xac]

// block 004dc7de
004dc7de  mov        al, byte ptr [ecx + esi]
004dc7e1  test       al, al
004dc7e3  je         0x4dc807

// block 004dc7e5
004dc7e5  mov        edi, dword ptr [edx + 0x88]
004dc7eb  and        eax, 0xff
004dc7f0  mov        eax, dword ptr [esp + eax*4 + 0x68]
004dc7f4  mov        dword ptr [edi + eax*4], ecx
004dc7f7  xor        eax, eax
004dc7f9  mov        al, byte ptr [ecx + esi]
004dc7fc  mov        edi, dword ptr [esp + eax*4 + 0x68]
004dc800  lea        eax, [esp + eax*4 + 0x68]
004dc804  inc        edi
004dc805  mov        dword ptr [eax], edi

// block 004dc807
004dc807  mov        eax, dword ptr [edx + 0x84]
004dc80d  inc        ecx
004dc80e  cmp        ecx, eax
004dc810  jb         0x4dc7de

// block 004dc812
004dc812  pop        edi
004dc813  pop        esi
004dc814  pop        ebp
004dc815  mov        al, 1
004dc817  pop        ebx
004dc818  add        esp, 0x98
004dc81e  ret        4
