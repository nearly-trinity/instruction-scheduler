# Instruction Scheduler

NATHAN EARLY AND JULIA ALFORD

In this project, you will implement the most interest part of an an instruction scheduling algorithm: building the dependence tree and computing the latency-weighted distance to root for each node. Your program will takes
as input a sequence of branch-free operations. You will transform the instructions by computing the virtual registers, and then outputint a
distance-labeled graph. To simplify matters, the test blocks do not read any input files. All data used by a given test block is contained in the test block or entered  using the ILOC simulator’s  `–i` option.


## ILOC Subset

Your input is a single basic block of code consisting of instructions
from the following subset of ILOC. All letters are case-sensitive. Note
that latencies have changed from Projects 1 \& 2. You *should* discard any  `nop` instructions, as they do not have to be scheduled.

|             Syntax                   | Meaning                 | Latency |
|:-----------------------------------|:------------------------|:--------|
| `load    r1       => r2`  | r2 $\gets$ MEM (r1)     | `3`      |
| `loadI   x        => r2`  | r2 $\gets$ x            | `1`      |
| `store   r1       => r2`  | MEM(r2) $\gets$ r1      | `3`      |
| `add     r1, r2   => r3`  | r3 $\gets$ r1 + r2      | `1`      |
| `sub     r1, r2   => r3`  | r3 $\gets$ r1 - r2      | `1`      |
| `mult    r1, r2   => r3`  | r3 $\gets$ r1 \* r2     | `2`      |
| `lshift  r1, r2   => r3`  | r3 $\gets$ r1 $\ll$ r2  | `1`      |
| `rshift  r1, r2   => r3`  | r3 $\gets$ r1 $\gg$ r2  | `1`      |
| `output  x             `  | prints MEM(x) to stdout | `1`      |
| `nop                   `  | idle for one cycle      | `1`      |

As in Project 1, all register names have an initial r followed
immediately by a non-negative integer. Leading zeros in the register
name are not significant. Arguments that do not begin with r, which
appear as x in the table above, are assumed to be positive integer
constants in the range 0 to $2^{31} - 1$. Spaces and tabs are treated as
whitespace. ILOC opcodes must be followed by whitespace. Whitespace
preceding and following all other symbols is optional. Whitespace is not
allowed within operation names, register names, or the assignment
symbol. A double slash (//) indicates that the rest of the line is a
comment and can be discarded. Empty lines may also be discarded.

# Project Components

Your project will proceed in the following steps, many of which can be
taken from Projects 1 & 2. For the milestone, you *should* have the front-end completed, and be able to construct a graph with register edges. For the full project you will include serialization edges, compute weighted distances, and output the graph.

## Front-End

First, you *must* scan and parse the ILOC. You *may* use the
scanner and parser from Project 1, fixing any errors that were
uncovered. You *must* annotate your intermediate representation to
store a label for each instruction. You *should* use integer labels, which *should* be printed with a leading 'n'.

Second, compute unique virtual registers for each value. Again, you *may*
use the algorithm from Project 2, fixing any errors that were uncovered.
You do not need to compute the next use of a value.

## Dependence Graph

Third, you *must* compute the dependence graph over the labels.
You are responsible for devising an algorithm to compute this graph. An
edge $(n_1, n_2)$ implies that $n_1$ depends on $n_2$ in some way, and
that $n_2$ must be completed before $n_1$ can be executed.. A *register
edge* exists if one of the source registers of $n_1$ is defined by
$n_2$. *Serialization edges* exist between instructions that deal with
memory, in the following cases.

1.  If $n_1$ is an `output` instruction, it depends upon
    the most recent `output` instruction, as values must be output in
    the same order.

2.  If $n_1$ is an  `output`,  `load`, or  `store` instruction ,
    there is a dependence on the most recent  `store` instruction.
    The previous  `store` instruction may modify the memory location
    $n_1$ accesses.

3.  If $n_1$ is a  `store` instruction, it depends upon the 
    most recent  `output` instruction and every previous  `load`
    instruction. The $n_1$ instruction may overwrite the memory location
    that the  `output` or  `load` instruction accesses. As an
    optimization, you can omit edges to  `load` instructions that occur
    before the most recent  `store` instruction.

## Latency-Weighted Distance to Root

Finally, compute the latency-weighted distance to a root for every edge. The graph will be acyclic, but you cannot assume it is weakly connected (only one node has no parents) or that it is a forest (every node has at most one parent). To do so, you will need to find the roots: instructions which have no
incoming edges. The weight of a root is the latency of its operation. Then, follow edges from parents to children. The weight of the child is the weight of the parent plus the latency of the child. Take care that, if a child has multiple parents, you keep the maximum weight of all
parents. You are again responsible for devising an algorithm to compute
these weights: I suggest a worklist. 

# Interface

Your code *must* compile on the `janus` machines, using a Makefile in that directory, to an executable named  `sched` in that directory. Your program must implement the following command line arguments.

```
-h  		Print a list of valid command-line arguments and descriptions to stdout and quit.

filename  	The last argument (assuming the -h flag is not passed) will be an input file containing an ILOC block.
```

The output will consist of three components. Each component maps a component label, on a single line, to by an indented map from node labels to values.

### `nodes:`
A list of unique labeled instructions in the block. The instruction *must* list the virtual registers, instead of the source registers. You *may* use any alphanumeric sequence as a label, but you *should* use increasing integers prefixed with 'n'. The syntax is  `label : instruction`. For instance, if the first
instruction is  `loadI 1024 => r1`, and the source register  `r1`
becomes the virtual register  `r12`, then the corresponding map might
be  `n0 : loadI 1024 => r12`. Each instruction *must* be on a new line. For debugging, you *may* include the original instruction as a comment following the renumbered instruction: i.e.
`n0 : loadI 1024 => r12 //loadI 1024 => r1`.

### `edges:`
The set of outgoing edges from each instruction in the format 
`label : { labels }`, where `label` is an instruction label from `nodes:`, and `labels` is a comma-separated list. Each edge *must*
point to a dependency of the labelled instruction. If there are no dependencies, then use the empty set  `{ }`. Each set *must* be on a new line.

### `weights:`
The latency-weighted distance from root for each instruction label, with the format `label : weight`. Each label *must* be on a new line.

# Example

An example file and corresponding output is provided below. Note the
serialization edges from  `n13` to  `n2, n5, n8` and  `n11`. Further,
note that  `n2`’s weight is  `13`, inherited from  `n3`, not  `6`
inherited from  `n13`.

**Input:**

    loadI 1024 => r0
    load r0 => r1
    add r1, r1 => r1
    loadI 1032 => r3
    load r3 => r2
    mult r1, r2 => r1
    loadI 1040 => r3
    load r3 => r2
    mult r1, r2 => r1
    loadI 1048 => r3
    load r3 => r2
    mult r1, r2 => r1
    store r1 => r0

**Output:**

    nodes:
        n1 : loadI 1024 => r1
        n2 : load r1 => r11
        n3 : add r11, r11 => r8
        n4 : loadI 1032 => r10
        n5 : load r10 => r9
        n6 : mult r8, r9 => r5
        n7 : loadI 1040 => r7
        n8 : load r7 => r6
        n9 : mult r5, r6 => r2
        n10 : loadI 1048 => r4
        n11 : load r4 => r3
        n12 : mult r2, r3 => r0
        n13 : store r0 => r1

    edges:
        n1 : { }
        n2 : { n1 }
        n3 : { n2 }
        n4 : { }
        n5 : { n4 }
        n6 : { n3, n5 }
        n7 : { }
        n8 : { n7 }
        n9 : { n6, n8 }
        n10 : { }
        n11 : { n10 }
        n12 : { n9, n11 }
        n13 : { n1, n12, n11, n8, n5, n2 }

    weights:
        n1 : 14
        n2 : 13
        n3 : 10
        n4 : 13
        n5 : 12
        n6 : 9
        n7 : 11
        n8 : 10
        n9 : 7
        n10 : 9
        n11 : 8
        n12 : 5
        n13 : 3
