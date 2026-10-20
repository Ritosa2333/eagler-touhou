
// block 00472927
00472927  mov        edi, edi
00472929  push       ebp
0047292a  mov        ebp, esp
0047292c  cmp        word ptr [ebp + 0x14], 0
00472931  jne        0x47293a

// block 00472933
00472933  mov        dword ptr [ebp + 0x14], 0x2800

// block 0047293a
0047293a  push       dword ptr [ebp + 0x14]
0047293d  push       0x46c941
00472942  push       0x46d04a
00472947  push       dword ptr [ebp + 0x10]
0047294a  push       dword ptr [ebp + 0xc]
0047294d  push       dword ptr [ebp + 8]
00472950  call       0x4821bc

// block 00472955
00472955  add        esp, 0x18
00472958  pop        ebp
00472959  ret        
