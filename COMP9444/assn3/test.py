import tensorflow as tf
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
sess = tf.InteractiveSession()

#t1 = tf.placeholder("float", [None, 4])
#t2 = tf.placeholder("float", [None, 1])

# A scalar index of a matrix x is a vector.
# Row 0 of x is [1,2,3,4,3,2,1], so gathering 0 returns it.
x = [ [1,2,3],[5,6,7]]
y=[[4],[8]]
# Gathering a vector index produces a scalar index for each element of the vector, and then concatenates them.
# Gathering [1,0] produces row 1 and row 0. Concatenating them flips the rows in the original matrix.





print(tf.concat([x,y],1).eval())



# [[ 2,3,4,3,2,1,0],
#  [ 1,2,3,4,3,2,1]]

# In general, each scalar index within the index tensor will produce an entire row in x.
#tf.gather(x, [[[1]],[[0]]]).eval()