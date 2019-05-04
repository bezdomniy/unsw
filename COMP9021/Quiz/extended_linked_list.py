# Written by Eric Martin for COMP9021

from linked_list import *

class ExtendedLinkedList(LinkedList):
    def __init__(self, L = None):
        super().__init__(L)

    def rearrange(self):
        if not self.head:
            return
        try:
            node=self.head
            end=node.next_node

            while True:
                position=end
                while position:
                    if position.value > node.value:
                        self.delete_value(position.value)
                        position.next_node=node.next_node
                        node.next_node = position
                        node=position

                    position = position.next_node

                node=end
                end=node.next_node
        except:
            pass
            
            
