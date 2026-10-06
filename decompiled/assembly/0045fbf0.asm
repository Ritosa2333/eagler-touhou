
// block 0045fbf0
0045fbf0  or         dword ptr [esi + 0x10], 1
0045fbf4  mov        edx, dword ptr [0x4ce9e4]
0045fbfa  mov        eax, dword ptr [0x4ce8f0]
0045fbff  mov        ecx, dword ptr [eax]
0045fc01  push       0
0045fc03  push       esi
0045fc04  push       0
0045fc06  push       edx
0045fc07  mov        edx, dword ptr [0x4ce9e0]
0045fc0d  push       1
0045fc0f  push       1
0045fc11  push       edx
0045fc12  mov        edx, dword ptr [0x4ce9dc]
0045fc18  push       edx
0045fc19  push       eax
0045fc1a  mov        eax, dword ptr [ecx + 0x5c]
0045fc1d  call       eax

// block 0045fc1f
0045fc1f  xor        ecx, ecx
0045fc21  cmp        dword ptr [0x4ce9e4], 0x16
0045fc28  sete       cl
0045fc2b  xor        eax, eax
0045fc2d  lea        ecx, [ecx + ecx + 2]
0045fc31  mov        dword ptr [esi + 0xc], ecx
0045fc34  ret        
