
// block 00488e30
00488e30  mov        edi, edi
00488e32  push       ebp
00488e33  mov        ebp, esp
00488e35  mov        eax, dword ptr [ebp + 0xc]
00488e38  cdq        
00488e39  push       0x1f
00488e3b  pop        ecx
00488e3c  and        edx, ecx
00488e3e  add        eax, edx
00488e40  mov        edx, dword ptr [ebp + 0xc]
00488e43  sar        eax, 5
00488e46  and        edx, 0x8000001f
00488e4c  jns        0x488e53

// block 00488e4e
00488e4e  dec        edx
00488e4f  or         edx, 0xffffffe0
00488e52  inc        edx

// block 00488e53
00488e53  sub        ecx, edx
00488e55  or         edx, 0xffffffff
00488e58  shl        edx, cl
00488e5a  mov        ecx, dword ptr [ebp + 8]
00488e5d  not        edx
00488e5f  test       dword ptr [ecx + eax*4], edx
00488e62  je         0x488e6e

// block 00488e64
00488e64  xor        eax, eax
00488e66  pop        ebp
00488e67  ret        

// block 00488e68
00488e68  cmp        dword ptr [ecx + eax*4], 0
00488e6c  jne        0x488e64

// block 00488e6e
00488e6e  inc        eax
00488e6f  cmp        eax, 3
00488e72  jl         0x488e68

// block 00488e74
00488e74  xor        eax, eax
00488e76  inc        eax
00488e77  pop        ebp
00488e78  ret        
