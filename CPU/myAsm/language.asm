main:

push 0
pop [0]
push 5 
pop [0]

push [0]
pop [1]

while_label1:
push [1]
push 1
jae while_end_label1:

push 1
push [1]
sub
pop [1]
push [0]
push [1]
mul
pop [0]
jump while_label1:
while_end_label1:

push [0]
out
hlt

