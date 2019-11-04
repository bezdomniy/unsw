size { select(U,V) : edge(U,V) } size.

subgraph(X,Y) :- edge(X,Y), not select(X,Y).
% 1 {start(X) : vertex(X)} 1.
% start(2).

% :- 2 {select(X,Y)}, vertex(X).
% :- 2 {select(X,Y)}, vertex(Y).

% Check graph is acyclic. 
closure(X,Y) :- subgraph(X,Y).
closure(X,Y) :- closure(X,Z), closure(Z,Y), X != Z, Z != Y.
:- closure(X,X), vertex(X).

% Check that each vertex in graph
inGraph(X) :- subgraph(X,_).
inGraph(Y) :- subgraph(_,Y).
:- vertex(X), not inGraph(X).

% Check if graph is connected
connected(X,Y):- subgraph(X,Y).
connected(X,Y):- connected(Y,X).
connected(X,Z):- subgraph(X,Y), connected(Y,Z).
connected(X,Z):- subgraph(Y,X), connected(Y,Z).
% % :- vertex(X), vertex(Y), not connected(X,Y).
:- not connected(X,Y), subgraph(X,_), subgraph(_,Y).

% Check that each vertex is reachable
% reachable(Y) :- start(X), select(X,Y).
% reachable(Y) :- reachable(X), select(X,Y).
% :- vertex(X), not reachable(X).

% #show start/1.
% #show subgraph/2.
#show select/2.