
// block 00412990
00412990  push       ecx
00412991  push       ebx
00412992  mov        ebx, dword ptr [0x4b43dc]
00412998  push       ebp
00412999  push       esi
0041299a  push       edi
0041299b  push       0x27b8
004129a0  call       0x46c9ea

// block 004129a5
004129a5  add        esp, 4
004129a8  test       eax, eax
004129aa  je         0x4129bc

// block 004129ac
004129ac  mov        ecx, dword ptr [esp + 0x18]
004129b0  push       ecx
004129b1  mov        edi, eax
004129b3  call       0x413230

// block 004129b8
004129b8  mov        ebp, eax
004129ba  jmp        0x4129be

// block 004129bc
004129bc  xor        ebp, ebp

// block 004129be
004129be  mov        eax, dword ptr [esp + 0x1c]
004129c2  mov        edx, dword ptr [eax]
004129c4  mov        dword ptr [ebp + 0x10a8], edx
004129ca  mov        ecx, dword ptr [eax + 4]
004129cd  mov        dword ptr [ebp + 0x10ac], ecx
004129d3  mov        edx, dword ptr [eax + 8]
004129d6  mov        dword ptr [ebp + 0x10b0], edx
004129dc  mov        ecx, dword ptr [eax + 0xc]
004129df  mov        dword ptr [ebp + 0x2644], ecx
004129e5  mov        edx, dword ptr [eax + 0x14]
004129e8  mov        dword ptr [ebp + 0x2648], edx
004129ee  mov        ecx, dword ptr [eax + 0x14]
004129f1  mov        dword ptr [ebp + 0x264c], ecx
004129f7  mov        edx, dword ptr [eax + 0x10]
004129fa  mov        dword ptr [ebp + 0x2660], edx
00412a00  mov        ecx, dword ptr [eax + 0x18]
00412a03  shl        ecx, 0x12
00412a06  xor        ecx, dword ptr [ebp + 0x26f8]
00412a0c  mov        dl, 1
00412a0e  and        ecx, 0x40000
00412a14  xor        dword ptr [ebp + 0x26f8], ecx
00412a1a  mov        ecx, dword ptr [0x4b0ca8]
00412a20  shl        dl, cl
00412a22  lea        esi, [eax + 0x20]
00412a25  lea        edi, [ebp + 0x127c]
00412a2b  mov        ecx, 0xc
00412a30  mov        byte ptr [ebp + 0x1024], dl

// block 00412a36
00412a36  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00412a38
00412a38  mov        ecx, dword ptr [ebp + 0x26e0]
00412a3e  xor        esi, esi
00412a40  lea        edi, [esi + 1]
00412a43  test       cl, 1
00412a46  jne        0x412a72

// block 00412a48
00412a48  fldz       
00412a4a  or         ecx, edi
00412a4c  fstp       dword ptr [ebp + 0x26d8]
00412a52  mov        dword ptr [ebp + 0x26d4], esi
00412a58  mov        dword ptr [ebp + 0x26d0], 0xfff0bdc1
00412a62  mov        dword ptr [ebp + 0x26dc], 0x4b2ed0
00412a6c  mov        dword ptr [ebp + 0x26e0], ecx

// block 00412a72
00412a72  fld        dword ptr [0x4a4014]
00412a78  mov        dword ptr [ebp + 0x26d4], 2
00412a82  fstp       dword ptr [ebp + 0x26d8]
00412a88  mov        dword ptr [ebp + 0x26d0], edi
00412a8e  mov        ecx, dword ptr [eax + 0x1c]
00412a91  shl        ecx, 0x19
00412a94  xor        ecx, dword ptr [ebp + 0x26f8]
00412a9a  mov        dword ptr [ebp + 0x1274], esi
00412aa0  and        ecx, 0x2000000
00412aa6  xor        dword ptr [ebp + 0x26f8], ecx
00412aac  cmp        dword ptr [eax + 0x14], 0x3e8
00412ab3  jl         0x412abf

// block 00412ab5
00412ab5  or         dword ptr [ebp + 0x26f8], 0x20000000

// block 00412abf
00412abf  lea        edx, [ebp + 0x1040]
00412ac5  push       edx
00412ac6  call       0x413840

// block 00412acb
00412acb  mov        eax, dword ptr [ebx + 0x74]
00412ace  mov        dword ptr [ebp + 0x27b4], eax
00412ad4  mov        ecx, dword ptr [ebx + 0x74]
00412ad7  and        ecx, edi
00412ad9  add        ecx, 3
00412adc  mov        dword ptr [ebp + 0x26b8], ecx
00412ae2  mov        ecx, 0x54
00412ae7  mov        dword ptr [ebp + 0x26bc], ecx
00412aed  cmp        dword ptr [ebp + 0x1264], edi
00412af3  jne        0x412b38

// block 00412af5
00412af5  mov        eax, dword ptr [ebp + 0x1268]
00412afb  cmp        eax, 0x39
00412afe  ja         0x412b38

// block 00412b00
00412b00  movzx      edx, byte ptr [eax + 0x412b9c]
00412b07  jmp        dword ptr [edx*4 + 0x412b88]

// block 00412b0e
00412b0e  mov        dword ptr [ebp + 0x26bc], ecx
00412b14  jmp        0x412b38

// block 00412b16
00412b16  mov        dword ptr [ebp + 0x26bc], 0x51
00412b20  jmp        0x412b38

// block 00412b22
00412b22  mov        dword ptr [ebp + 0x26bc], 0x5a
00412b2c  jmp        0x412b38

// block 00412b2e
00412b2e  mov        dword ptr [ebp + 0x26bc], 0x57

// block 00412b38
00412b38  mov        dword ptr [ebp + 0x26c0], esi
00412b3e  lea        eax, [ebp + 0x12c0]
00412b44  cmp        dword ptr [ebx + 0x68], esi
00412b47  jne        0x412b4e

// block 00412b49
00412b49  mov        dword ptr [ebx + 0x68], eax
00412b4c  jmp        0x412b67

// block 00412b4e
00412b4e  mov        ecx, dword ptr [ebx + 0x6c]
00412b51  mov        edx, dword ptr [ecx + 4]
00412b54  cmp        edx, esi
00412b56  je         0x412b61

// block 00412b58
00412b58  mov        dword ptr [eax + 4], edx
00412b5b  mov        edx, dword ptr [ecx + 4]
00412b5e  mov        dword ptr [edx + 8], eax

// block 00412b61
00412b61  mov        dword ptr [ecx + 4], eax
00412b64  mov        dword ptr [eax + 8], ecx

// block 00412b67
00412b67  add        dword ptr [ebx + 0x70], edi
00412b6a  mov        dword ptr [ebx + 0x6c], eax
00412b6d  mov        eax, dword ptr [ebx + 0x74]
00412b70  mov        dword ptr [ebx + 0x78], eax
00412b73  inc        eax
00412b74  mov        dword ptr [ebx + 0x74], eax
00412b77  cmp        eax, esi
00412b79  mov        eax, ebp
00412b7b  jne        0x412b80

// block 00412b7d
00412b7d  mov        dword ptr [ebx + 0x74], edi

// block 00412b80
00412b80  pop        edi
00412b81  pop        esi
00412b82  pop        ebp
00412b83  pop        ebx
00412b84  pop        ecx
00412b85  ret        8
