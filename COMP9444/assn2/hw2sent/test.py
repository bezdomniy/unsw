import glob
import os

dir = os.path.dirname(__file__)
pos_set1 = set( [os.path.basename(x) for x in glob.glob(os.path.join(dir,'data2/pos/*.txt'))])


dir = os.path.join(dir,'test/')

pos_set2 = set( [os.path.basename(x) for x in glob.glob(os.path.join(dir,'train/data2/pos/*.txt'))])

print(len(pos_set1-pos_set2))