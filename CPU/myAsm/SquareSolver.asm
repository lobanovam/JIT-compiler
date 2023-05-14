in
copy
pop rax

push 0
je linear:

jump quadratic:

linear:
    in
    copy
    pop rbx

    push 0
    je noA_noB:

    push rbx

    push -1
    in
    mul

    div
    out
    hlt

quadratic:
    in
    copy
    pop rbx

    push 0
    je noB:

    call disc:

    call calcRoots:

    out
    out

    hlt

disc:
    in
    copy
    pop rcx

    push rax
    copy
    pop rax

    push 4
    mul
    mul

    push rbx
    copy
    copy
    pop rbx
    mul

    sub
    copy

    push 0
    ja abort:

    sqrt
    pop rdx

    ret

calcRoots:
    push rax
    copy
    pop rax
    push 2
    mul

    push rdx
    copy
    pop rdx

    push rbx
    copy
    pop rbx

    push -1
    mul

    sub

    div

    push rax
    copy
    pop rax
    push 2
    mul

    push rdx
    copy
    pop rdx

    push rbx
    copy
    pop rbx

    push -1
    mul

    add

    div

    ret

noB:
    in
    copy
    pop rcx

    push 0
    je noB_noC:

    push rax

    push -1
    push rcx
    mul
    copy

    push 0
    ja abort:

    div
    copy
    sqrt
    out

    sqrt
    push -1
    mul
    out
    hlt

noB_noC:
    push 0:
    out
    hlt

noA_noB:
    in
    push 0
    je infR:
    jump abort:

infR:
    push 228228
    out
    hlt

abort:
    push 696969
    out
    hlt

