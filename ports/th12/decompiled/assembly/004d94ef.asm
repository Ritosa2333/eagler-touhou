
// block 004d94ef
004d94ef  imul       eax, dword ptr [eax], 0
004d94f2  and        byte ptr [eax], al
004d94f4  add        byte ptr [eax], al
004d94f6  pushal     
004d94f7  add        byte ptr [eax], al
004d94f9  add        byte ptr [edx], dl
004d94fb  add        byte ptr [eax], al
004d94fd  add        byte ptr [eax + eax], ch
004d9500  add        byte ptr [eax], al
004d9502  add        byte ptr [eax], al
004d9504  add        byte ptr [eax], al
004d9506  add        byte ptr [eax], al
004d9508  add        byte ptr [eax], al
004d950a  add        byte ptr [eax], al
004d950c  add        byte ptr [eax], ah
004d950f  add        al, ah

// block 004d9512
004d9512  popal      
004d9513  popal      
004d9515  je         0x4d9578

// block 004d9517
004d9517  add        byte ptr [eax], al
004d9519  add        byte ptr [eax], dl
004d951b  add        byte ptr [eax], al
004d951d  add        byte ptr [eax], al
004d9523  add        byte ptr [eax], al
004d9525  add        byte ptr [esi], bh
004d9527  add        byte ptr [eax], al
004d9529  add        byte ptr [eax], al
004d952b  add        byte ptr [eax], al
004d952d  add        byte ptr [eax], al
004d952f  add        byte ptr [eax], al
004d9531  add        byte ptr [eax], al
004d9533  add        byte ptr [eax], al
004d9535  inc        eax
004d9536  add        byte ptr [eax], al
004d9538  loopne     0x4d953a
