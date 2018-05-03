xterm -hold -title "Peer 1" -e "java CircularDHT/DHTPeer 1 3 4" &
xterm -hold -title "Peer 3" -e "java CircularDHT/DHTPeer 3 4 5" &
xterm -hold -title "Peer 4" -e "java CircularDHT/DHTPeer 4 5 1" &
xterm -hold -title "Peer 5" -e "java CircularDHT/DHTPeer 5 1 3" &

