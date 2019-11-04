% Petersen Graph
vertex(1..10).
edge(1,2). edge(2,3). edge(3,4). edge(4,5). edge(5,1).
edge(6,8). edge(8,10). edge(10,7). edge(7,9). edge(9,6).
edge(1,6). edge(2,7). edge(3,8). edge(4,9). edge(5,10).
edge(U, V) :- edge(V, U).

% #const size=4.
#show select/1.

#show select/2.


%%%%%%%% example output of 'clingo --const size=3 coloring.lp petersen.lp
% Answer: 1
% select(1,2) select(2,3) select(5,3) select(6,3) select(3,1) select(7,2) select(4,2) select(8,2) select(9,1) select(10,1)
% SATISFIABLE
%%%%%%%% example output of 'clingo --const size=2 coloring.lp petersen.lp
% UNSATISFIABLE

%%%%%%%% example output of 'clingo --const size=4 independent.lp petersen.lp
% Answer: 1
% select(1) select(3) select(10) select(9)
% SATISFIABLE
%%%%%%%% example output of 'clingo --const size=5 independent.lp petersen.lp
% UNSATISFIABLE

%%%%%%%% example output of 'clingo --const size=15 feedback.lp petersen.lp
% Answer: 1
% select(2,3) select(3,4) select(4,5) select(5,1) select(8,10) select(10,7) select(7,9) select(9,6) select(1,6) select(2,7) select(3,8) select(4,9) select(5,10) select(8,6) select(2,1)
% SATISFIABLE
%%%%%%%% example output of 'clingo --const size=14 feedback.lp petersen.lp
% UNSATISFIABLE



