
// block 004da919
004da919  inc        ebp
004da91a  out        0x12, al
004da91c  popal      
004da91d  cmp        eax, 0xc988d3fc
004da922  and        dword ptr [ebx - 0x4efea4f5], 0x34c42433
004da92c  inc        edi
004da92d  push       edi
004da92e  in         eax, 0xbb
004da930  lahf       
