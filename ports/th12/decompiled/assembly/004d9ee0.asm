
// block 004d9ee0
004d9ee0  aam        0x1d
004d9ee2  dec        edx
004d9ee3  fiadd      dword ptr [esp + ebp*4 - 0x29]
004d9ee7  mov        dword ptr [ebx + ebx*4 + 0x6d497985], edx
004d9eee  ja         0x4d9f49

// block 004d9f49
004d9f49  push       esi
004d9f4a  in         al, dx
004d9f4b  popfd      
004d9f4d  imul       edx, dword ptr [edi + 0x30ab17cc], 0x444a4978
004d9f57  das        
004d9f58  add        ebx, dword ptr [eax + edx + 0xd45ce14]
004d9f5f  hlt        
