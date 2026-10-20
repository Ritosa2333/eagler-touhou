
// block 0044cb50
0044cb50  lea        ecx, [esi + esi*2]
0044cb53  push       edi
0044cb54  mov        edi, dword ptr [ecx*4 + 0x4b4540]
0044cb5b  lea        eax, [edx + edx*2]
0044cb5e  mov        dword ptr [eax*4 + 0x4b4540], edi
0044cb65  mov        eax, dword ptr [ecx*4 + 0x4b4540]
0044cb6c  lea        eax, [eax + eax*2]
0044cb6f  add        eax, eax
0044cb71  add        eax, eax
0044cb73  pop        edi
0044cb74  cmp        dword ptr [eax + 0x4b4548], esi
0044cb7a  jne        0x44cb8e

// block 0044cb7c
0044cb7c  mov        dword ptr [eax + 0x4b4548], edx
0044cb82  mov        dword ptr [ecx*4 + 0x4b4540], 0
0044cb8d  ret        

// block 0044cb8e
0044cb8e  mov        dword ptr [eax + 0x4b4544], edx
0044cb94  mov        dword ptr [ecx*4 + 0x4b4540], 0
0044cb9f  ret        
