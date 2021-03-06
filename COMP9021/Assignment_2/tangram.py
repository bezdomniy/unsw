from itertools import product
from math import atan2

class Point(object):
    def __init__(self,x,y):
        self.x = int(x)
        self.y = int(y)
    def __repr__(self):
        return 'Point({},{})'.format(self.x,self.y)
    def __eq__(self,other):
        return self.x==other.x and self.y==other.y
    def __hash__(self):
        return hash(tuple([self.x,self.y]))

class Line(object):
    def __init__(self,p1,p2):
        self.p1 = p1
        self.p2 = p2
    def __repr__(self):
        return 'Line({},{})'.format(self.p1,self.p2)
    def __eq__(self,other):
        return (self.p1==other.p1 and self.p2==other.p2) or (self.p1==other.p2 and self.p2==other.p1)
    def __hash__(self):
        p1=self.p1
        p2=self.p2
        if p1.x <= p2.x:
            x=p1
            y=p2
            if p1.x == p2.x:
                if p1.y <= p2.y:
                    x=p1
                    y=p2
                else:
                    x=p2
                    y=p1
        else:
            x=p2
            y=p1
        return hash(tuple([x,y]))

    def line_on_line(self,other):
        
        if ((other.p2.x < max(self.p1.x, self.p2.x) and other.p2.x > min(self.p1.x, self.p2.x) and other.p2.y < max(self.p1.y, self.p2.y) and other.p2.y > min(self.p1.y, self.p2.y)) \
           or other.p1.x < max(self.p1.x, self.p2.x) and other.p1.x > min(self.p1.x, self.p2.x) and other.p1.y < max(self.p1.y, self.p2.y) and other.p1.y > min(self.p1.y, self.p2.y)) \
           or (self==other):
##                print('true',self,other)
                return True
##        print('false',self,other)
        return False

class Shape(object):
    def __init__(self,Points,Colour):
        self.coordinates = Points
        self.colour = Colour

        ## Create line segments
        self.segments=[]
        for i in range(len(self.coordinates)):
            j = (i + 1) % len(self.coordinates)
            self.segments.append(Line(self.coordinates[i],self.coordinates[j]))

    def __repr__(self):
       return '{}'.format(self.coordinates)

    def __eq__(self,other):
        return set(self.coordinates)==set(other.coordinates) and self.colour == other.colour

        
    def area(self):
        sum_of_values = 0
        for i in range(len(self.coordinates)):
            j = (i + 1) % len(self.coordinates)
            sum_of_values+=self.coordinates[i].x*self.coordinates[j].y-self.coordinates[j].x*self.coordinates[i].y
        return abs(sum_of_values//2)

    def centre(self):
        min_x=min([p.x for p in self.coordinates])
        min_y=min([p.y for p in self.coordinates])
        max_x=max([p.x for p in self.coordinates])
        max_y=max([p.y for p in self.coordinates])
        
        x = min_x +(max_x - min_x)//2
        y = min_y +(max_y - min_y)//2

        return Point(x,y)

    def check_connecting(self,other):
        lines1=[]
        lines2=[]
        for i in range(len(self.coordinates)):
            j = (i + 1) % len(self.coordinates)
            lines1.append(Line(self.coordinates[i],self.coordinates[j]))
        for i in range(len(other.coordinates)):
            j = (i + 1) % len(other.coordinates)
            lines2.append(Line(other.coordinates[i],other.coordinates[j]))
        for i in lines1:
            for j in lines2:
                if i.line_on_line(j) or j.line_on_line(i):
                    return j
        return False



    def bring_to_origin(self):
        points=[]
        min_x=min([p.x for p in self.coordinates])
        min_y=min([p.y for p in self.coordinates])
        for p in self.coordinates:
            new_x = p.x-min_x
            new_y = p.y-min_y
            points.append(Point(new_x,new_y))
        return Shape(points,self.colour)
 
    def rotate(self,times):
        if times % 4 == 0:
            return self
        points=[]
        for p in self.coordinates:
            new_x = p.y
            new_y = -p.x
            points.append(Point(new_x,new_y))
        if times % 4 == 1:
            return Shape(points,self.colour)
        if times % 4 == 2:
            return Shape(points,self.colour).rotate(1)
        if times % 4 == 3:
            return Shape(points,self.colour).rotate(2)

    def flip(self,times):
        if times % 2 == 0:
            return self
        points=[]
        for p in self.coordinates:
            new_x = -p.x
            points.append(Point(new_x,p.y))
        return Shape(points,self.colour)
                    
def on_line(p1,p2,p3,include_ends=True):
    if include_ends==True:
        if  p2.x <= max(p1.x, p3.x) and p2.x >= min(p1.x, p3.x) and p2.y <= max(p1.y, p3.y) and p2.y >= min(p1.y, p3.y):
            return True
        return False
    else:
        if  p2.x < max(p1.x, p3.x) and p2.x > min(p1.x, p3.x) and p2.y < max(p1.y, p3.y) and p2.y > min(p1.y, p3.y):
            return True
        return False

def check_orientation(p1,p2,p3):
    o = (p2.y-p1.y) * (p3.x-p2.x) - (p2.x-p1.x) * (p3.y-p2.y)

    if o == 0:
        return 0
    elif o > 0:
        return 1
    else:
        return 2

def check_intersect(line1, line2):
    o1 = check_orientation(line1.p1, line1.p2, line2.p1)
    o2 = check_orientation(line1.p1, line1.p2, line2.p2)
    o3 = check_orientation(line2.p1, line2.p2, line1.p1)
    o4 = check_orientation(line2.p1, line2.p2, line1.p2)

    if o1 != o2 and o3 != o4:
        return True
    if (o1 == 0 and on_line(line1.p1, line2.p1, line1.p2)):
        return True
    if (o2 == 0 and on_line(line1.p1, line2.p2, line1.p2)):
        return True
    if (o3 == 0 and on_line(line2.p1, line1.p1, line2.p2)):
        return True
    if (o4 == 0 and on_line(line2.p1, line1.p1, line2.p2)):
        return True
 
    return False

def is_valid(shapes):
    for shape in shapes:
        ##  Check if convex or if any 3 consecutive points are colinear
        flag=0
        p=shape.coordinates
        for i in range(len(p)):
            j = (i + 1) % len(p)
            k = (i + 2) % len(p)

            z  = check_orientation(p[i],p[j],p[k])
            flag |= z

            if flag == 3 or z == 0:
                return False

        ##  Check if self-intersecting
        for i in range(len(shape.segments)):
            for j in range(len(shape.segments)):
                if not(( j-1 <= i <= j+1) or (i == len(shape.segments)-1 and j==0) or (j == len(shape.segments)-1 and i==0)):
                    if check_intersect(shape.segments[i],shape.segments[j]) == True:
                        return False
    if flag != 0:
        return True
    else:
        return False


def available_coloured_pieces(file):
    lines = file.readlines()
    shapes=[]
    for i in range(len(lines)):
        if lines[i].replace(" ","")[0:5] != "<path":
            continue
        
        flag=0
        colour=[]
        for c in lines[i][::-1]:
            if c.isalpha() and flag==0:
                flag=1
            elif not c.isalpha() and flag==1:
                break
            if flag==1:
                colour.append(c)
        colour="".join(colour[::-1])

        string = lines[i].split()
        points = []
        for j in range(len(string)-1):
            if string[j].isdigit() and string[j+1].isdigit():
                points.append(Point(string[j],string[j+1]))
        shapes.append(Shape(Points=points,Colour=colour))
    return shapes

def are_identical_sets_of_coloured_pieces(set_1,set_2):
    if len(set_1) != len(set_2):
        return False
    
    count=0
    for i in set_1:
        for j in set_2:
            times=product(range(4),repeat=4)
            while True:
                if i.colour != j.colour:
                    break
                try:
                    k=next(times)
                except StopIteration:
                    break
                if k[0] == k[1] and not (k[0] == 0 and k[1] == 0):
                    continue
                if i.rotate(k[0]).flip(k[2]).bring_to_origin()==j.rotate(k[1]).flip(k[3]).bring_to_origin():
                    count+=1
                    break
                    
    if count != len(set_1):
        return False
    return True



def is_solution(tangram,shape):
    shape_area=shape[0].area()
    tangram_area=0
    for i in tangram:
        tangram_area+=i.area()
        centre=i.centre()
        i.coordinates.sort(key=lambda p: atan2(centre.x-p.x,centre.y-p.y))
##    if tangram_area != shape_area:
##        return False


    new_shape=None
    while tangram != []:
        for i in tangram:
            if new_shape is None:
                new_shape = Shape(i.coordinates,"Grey")
                tangram.remove(i)
                print('new',new_shape.coordinates)
                continue

            lines=[]

            for p in range(len(new_shape.coordinates)):
                q = (p + 1) % len(new_shape.coordinates)
                lines.append(Line(new_shape.coordinates[p],new_shape.coordinates[q]))
            
            connecting_line = new_shape.check_connecting(i)       
            if type(connecting_line) is Line:
                for j in range(len(lines)):
                    cond1=on_line(lines[j].p1,connecting_line.p1,lines[j].p2)
                    cond2=on_line(lines[j].p1,connecting_line.p2,lines[j].p2)
                    if cond1 or cond2:
                        if lines[j].p1 in i.coordinates:
                            print('next_shape',i.colour,i.coordinates)
                            index=i.coordinates.index(lines[j].p1)
                            rearranged_shape=i.coordinates[index + 1:] + i.coordinates[:index + 1]
                            new_coord=sum([rearranged_shape if x==rearranged_shape[0] else [x] for x in new_shape.coordinates],[])

                            new_shape=Shape(new_coord,"Grey")
                            break
                        else:
                            new_coord=[[x] for x in new_shape.coordinates]
                            print('n',new_coord)
                            if cond1:
                                index=i.coordinates.index(connecting_line.p1)

                            elif cond2:
                                index=i.coordinates.index(connecting_line.p2)

                            rearranged_shape=i.coordinates[index + 1:] + i.coordinates[:index + 1]
                                
                            new_coord=sum(new_coord.insert(new_coord.index([lines[j].p2]),rearranged_shape),[])
                            new_shape=Shape(new_coord,"Grey")
                            break

                        
                tangram.remove(i)
                    
                print('a',new_shape.coordinates)
                print(len(tangram))
            else:
##                print(new_shape)
                continue

    print(new_shape)


file = open('tangram_a_1_a.xml')
file2 = open('shape_a_1.xml')

coloured_pieces = available_coloured_pieces(file)
coloured_pieces2 = available_coloured_pieces(file2)

print(is_solution(coloured_pieces,coloured_pieces2))

