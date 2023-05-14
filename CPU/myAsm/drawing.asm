push 30
pop rfx
push 15
pop rax
push 15
pop rbx
push 10
pop rcx

push 0
pop rdx
push 0
pop rex

jump doCircle:

doCircle:
    push rdx
    copy
    pop rdx

    push rax
    copy
    pop rax

    sub
    copy
    mul

    push rex
    copy
    pop rex

    push rbx
    copy
    pop rbx

    sub
    copy
    mul

    add

    push rcx
    copy
    pop rcx

    copy
    mul

    jae setMem:

    jump update:


setMem:
    push rex
    copy
    pop rex

    push rfx
    copy
    pop rfx

    mul

    push rdx
    copy
    pop rdx

    add

    pop rgx

    push 1
    pop [rgx]

    RAM_PRINT

    jump update:

update:

    push rdx
    push 1
    add
    copy

    push rfx
    copy
    pop rfx

    je newLine:

    pop rdx

    jump doCircle:


newLine:
    push 0
    pop rdx

    push rex
    push 1
    add
    copy

    push rfx
    copy
    pop rfx

    je abort:

    pop rex

    jump doCircle:

abort:
    RAM_PRINT
    hlt




















