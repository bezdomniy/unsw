color(1..size). % there are "size" different colors.

1 { select(V,C) : color(C) } 1 :- vertex(V). % each vertex is assigned exactly one C such that C is a color
:- edge(U,V), select(U,C), select(V,C). % no adjacent vertices can be assigned the same color
