
// block 00470eee
00470eee  mov        edi, edi
00470ef0  push       ebp
00470ef1  mov        ebp, esp
00470ef3  push       esi
00470ef4  push       dword ptr [0x4b3d60]
00470efa  call       0x4751de

// block 00470eff
00470eff  push       dword ptr [ebp + 8]
00470f02  mov        esi, eax
00470f04  call       0x475163

// block 00470f09
00470f09  pop        ecx
00470f0a  pop        ecx
00470f0b  mov        dword ptr [0x4b3d60], eax
00470f10  mov        eax, esi
00470f12  pop        esi
00470f13  pop        ebp
00470f14  ret        
