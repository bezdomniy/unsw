xterm -hold -title "Peer 1" -e "java cdht 1 3 4" &
xterm -hold -title "Peer 3" -e "java cdht 3 4 5" &
xterm -hold -title "Peer 4" -e "java cdht 4 5 8" &
xterm -hold -title "Peer 5" -e "java cdht 5 8 10" &
xterm -hold -title "Peer 8" -e "java cdht 8 10 12" &
xterm -hold -title "Peer 10" -e "java cdht 10 12 15" &
xterm -hold -title "Peer 12" -e "java cdht 12 15 1" &
xterm -hold -title "Peer 15" -e "java cdht 15 1 3" &
