main:

push 0
pop [0]
in
pop [0]

push 0
pop [1]
in
pop [1]

push 0
pop [2]
in
pop [2]
push [0]
push 0
jne if_label0:

push [2]
push [1]
call firstNull:
pop [0]
jump else_label0:

if_label0:
push [2]
push [1]
push [0]
call full:
pop [0]

else_label0:
hlt

firstNull:
pop [3]
pop [4]
push [3]
push 0
jne if_label1:

push [4]
call secondNull:
pop [3]
push 0
ret

jump else_label1:

if_label1:
push [4]
push -1
mul
pop [4]
push [3]
push [4]
div
pop [3]

else_label1:
push [3]
out
push 0
ret

secondNull:
pop [5]
push [5]
push 0
jne if_label2:

push 6969
out
jump else_label2:

if_label2:
push 228
out

else_label2:
push 0
ret

full:
pop [6]
pop [7]
pop [8]

push 4
push [6]
mul
push [8]
mul
push [7]
push [7]
mul
sub
pop [9]
push [9]
push 0
jbe if_label3:

push 228
out
jump else_label3:

if_label3:
push [9]
sqrt
pop [9]
push -1
push [7]
mul
pop [7]
push 2
push [6]
mul
pop [6]

push [6]
push [9]
push [7]
sub
div
pop [10]

push [6]
push [7]
push [9]
add
div
pop [11]
push [10]
out
push [11]
out

else_label3:
push 0
ret

