from itertools import product
from math import atan2
from random import choice, randrange,shuffle
from ring import *
from copy import deepcopy
import time

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

## Checks if a point is inside a shape
    def is_in(self,other):
        shape_in=False
        n_other_points = len(other.coordinates)
        p_self = self
        point_in=False
        i=0
        j=n_other_points-1
        while i < n_other_points:
            if on_line(other.coordinates[i],p_self,other.coordinates[j]):
                point_in = True
                break
            elif (other.coordinates[i].y > p_self.y) != (other.coordinates[j].y > p_self.y) and \
               (p_self.x < (other.coordinates[j].x - other.coordinates[i].x) * (p_self.y-other.coordinates[i].y) / \
               (other.coordinates[j].y - other.coordinates[i].y) + other.coordinates[i].x):
                point_in= not point_in
            i,j=i+1,i
        if point_in:
            shape_in = True
        else:
            shape_in = False
            
        return shape_in

class Line(object):
    def __init__(self,p1,p2):
        self.p1 = p1
        self.p2 = p2

        try:
            self.gradient=(p1.y-p2.y)/(p1.x-p2.x)
        except ZeroDivisionError:
            self.gradient= None
            
    def __repr__(self):
        return 'Line({},{})'.format(self.p1,self.p2)
    def __eq__(self,other):
        return (self.p1==other.p1 and self.p2==other.p2) or (self.p1==other.p2 and self.p2==other.p1)

## returns all the integer coordinates that are on the segment
    def get_points(self):
        points=[]
        max_x=max(self.p1.x,self.p2.x)
        min_x=min(self.p1.x,self.p2.x)
        if self.gradient is not None:
            for x in range(min_x,max_x):
                y=self.gradient*(x-self.p1.x)+self.p1.y
                if y.is_integer():
                    points.append(Point(x,y))
        else:
            max_y=max(self.p1.y,self.p2.y)
            min_y=min(self.p1.y,self.p2.y)
            for y in range(min_y,max_y):
                points.append(Point(self.p1.x,y))

        return points

## checks if the line overlaps with another given line
    def line_on_line(self,other):
        if self==other:
            return True
        if on_line(self.p1,other.p1,self.p2,include_ends=False) or \
           on_line(self.p1,other.p2,self.p2,include_ends=False) or \
           on_line(other.p1,self.p1,other.p2,include_ends=False) or \
           on_line(other.p1,self.p2,other.p2,include_ends=False):
            return True
        return False


## Checks if points are colinear, or if p3 is to the left or right of segment p1,p2
def check_orientation(p1,p2,p3):
    o = (p2.y-p1.y) * (p3.x-p2.x) - (p2.x-p1.x) * (p3.y-p2.y)

    if o == 0:
        return 0
    elif o > 0:
        return 1
    else:
        return 2

## Checks if a point is on a segment
def on_line(p1,p2,p3,include_ends=True):
    if p2.x <= max(p1.x, p3.x) and p2.x >= min(p1.x, p3.x) and p2.y <= max(p1.y, p3.y) and p2.y >= min(p1.y, p3.y) and \
           check_orientation(p1,p2,p3) == 0:
        if include_ends:
            return True
        if p1 != p2 and p3 != p2:
            return True
        return False
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
       return '{}'.format(self.colour)

    def __eq__(self,other):
        return set(self.coordinates)==set(other.coordinates) and self.colour == other.colour

    def __hash__(self):
        return hash(tuple(self.coordinates))

## returns the centroid of the shape
    def centre(self):
        min_x=min([p.x for p in self.coordinates])
        min_y=min([p.y for p in self.coordinates])
        max_x=max([p.x for p in self.coordinates])
        max_y=max([p.y for p in self.coordinates])
        
        x = min_x +(max_x - min_x)//2
        y = min_y +(max_y - min_y)//2

        return Point(x,y)

## returns the line which connects 2 shape, otherwise returns false
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

## returns true if the shape is convex
    def is_convex(self):
        flag=0
        p=self.coordinates
        for i in range(len(p)):
            j = (i + 1) % len(p)
            k = (i + 2) % len(p)

            z  = check_orientation(p[i],p[j],p[k])
            flag |= z

            if flag == 3:
                return False
        return True

## returns true if points in shape are arranged clockwise
    def check_clockwise(self):
        p=self.coordinates
        output=0
        
        for i in range(len(p)):

            j = (i + 1) % len(p)
            k = (i + 2) % len(p)

            run = (p[j].x - p[i].x) * (p[k].y - p[j].y) - \
                  (p[j].y - p[i].y) * (p[k].x - p[j].x)

            if run < 0:
                output-=1
            elif run > 0:
                output+=1

        if output < 0:
            return False
        elif output > 0:
            return True
        return

## sorts the coordinates in a shape clockwise or counterclockwise (sometimes may not work on concave shapes)
    def sort_coordinates(self,clockwise=False):
        centre=self.centre()
        return Shape(sorted(self.coordinates,key=lambda p: atan2(centre.x-p.x,centre.y-p.y),reverse=clockwise),Colour=self.colour)

## checks if there are duplicate coordinates in a shape
    def self_intersecting(self):
        return len(self.coordinates) == len(set(self.coordinates))

## function used to clip the shape with each addition of a piece
    def remove(self,piece,connection):
        new_coordinates = Ring(self.coordinates)
        sorted_piece = Ring(piece.sort_coordinates(clockwise=True).coordinates)

        self_c = [x for x in self.segments if x.p2 == connection][0]
        piece_c = [y.pop(y.index(connection)-1) for y in [[x.p1,x.p2] for x in piece.segments if x.p1 == connection or x.p2==connection]]

        try:
            if on_line(self_c.p1,piece_c[0],self_c.p2,include_ends=False) or on_line(self_c.p1,piece_c[1],self_c.p2,include_ends=False):
                sorted_piece=sorted_piece[sorted_piece._data.index(connection)+1:]

            else:
                sorted_piece=sorted_piece[sorted_piece._data.index(connection):]
        except ValueError:
            return self,False

        anchor_index=new_coordinates._data.index(connection)

        if new_coordinates[anchor_index+1] == sorted_piece[-1]:
            sorted_piece.pop()
        
        new_coordinates[anchor_index:anchor_index:0]= sorted_piece

        final_coordinates=[]
        for i in range(len(new_coordinates)):
            line1=Line(new_coordinates[i-1],new_coordinates[i])
            line2=Line(new_coordinates[i],new_coordinates[i+1])

            if (line1.gradient!=line2.gradient) and new_coordinates[i] != new_coordinates[i+1]:
                try:
                    if final_coordinates[-1] != new_coordinates[i]:
                        final_coordinates.append(new_coordinates[i])
                except IndexError:
                    final_coordinates.append(new_coordinates[i])

        output=Shape(final_coordinates,Colour=self.colour)

        return output, True

## returns the area of a polygon
    def area(self):
        sum_of_values = 0
        for i in range(len(self.coordinates)):
            j = (i + 1) % len(self.coordinates)
            sum_of_values+=self.coordinates[i].x*self.coordinates[j].y-self.coordinates[j].x*self.coordinates[i].y
        return abs(sum_of_values//2)

## shifts the piece to a given point using a given coordinate in the piece as an anchor
    def bring_to(self,point=Point(0,0),side="NW"):
        points=[]

        if side=="NW":
            self_y = min([p.y for p in self.coordinates])
            self_x = min([p.x for p in self.coordinates if p.y==self_y])
        if side=="NE":
            self_y = min([p.y for p in self.coordinates])
            self_x = max([p.x for p in self.coordinates if p.y==self_y])
        if side=="SW":
            self_y = max([p.y for p in self.coordinates])
            self_x = min([p.x for p in self.coordinates if p.y==self_y])
        if side=="SE":
            self_y = max([p.y for p in self.coordinates])
            self_x = max([p.x for p in self.coordinates if p.y==self_y])
            
        change_x=point.x-self_x
        change_y=point.y-self_y
        
        for p in self.coordinates:
            new_x = p.x+change_x
            new_y = p.y+change_y
            points.append(Point(new_x,new_y))
        return Shape(points,self.colour)

## flips the piece
    def flip(self):
        points=[]
        for p in self.coordinates:
            new_x = p.x
            new_y = -p.y
            points.append(Point(new_x,new_y))
        return Shape(points,self.colour)

## rotates the piece around the origin n times
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

  
## returns the NW, NE, SW, or SE corner of the shape
    def get_point(self,point):
        if point == "NW":
            y=min([p.y for p in self.coordinates])
            x=min([p.x for p in self.coordinates if p.y == y])
            return Point(x,y)
        if point == "NE":
            y=min([p.y for p in self.coordinates])
            x=max([p.x for p in self.coordinates if p.y == y])
            return Point(x,y)
        if point == "SW":
            y=max([p.y for p in self.coordinates])
            x=min([p.x for p in self.coordinates if p.y == y])
            return Point(x,y)
        if point == "SE":
            y=max([p.y for p in self.coordinates])
            x=max([p.x for p in self.coordinates if p.y == y])
            return Point(x,y)

## returns true if all the points in the shape are inside the other given shape
    def is_in(self,other):
        shape_in=False
        for p_self in self.coordinates:
            point_in=False
            for line in other.segments:
                if on_line(line.p1,p_self,line.p2):
                    point_in = True
                    break
                
                elif (line.p1.y > p_self.y) != (line.p2.y > p_self.y) and \
                   ((p_self.x < (line.p2.x - line.p1.x) * (p_self.y - line.p1.y) / \
                   (line.p2.y - line.p1.y) + line.p1.x)):
                    point_in= not point_in

            if point_in:
                shape_in = True
            else:
                shape_in = False
                break

        if shape_in and self._outside_line(other):
            shape_in=False
            
        return shape_in

## returns true if any part of a segment of the shape is outside of the other given shape
    def _outside_line(self,other):
        check=False
            
        for self_i in self.segments:
            points = self_i.get_points()

            for point in points:
                if not point.is_in(other):
                    return True
        return check

def available_coloured_pieces(file):
    lines = file.readlines()
    shapes=[]

    for i in range(len(lines)):
        if lines[i].replace(" ","").replace("\t","")[0:5] != "<path":
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

        if len(lines)-2 == 1:
            shape=Shape(Points=points,Colour=colour)
            if shape.check_clockwise():
                
                points=points[::-1]
                return Shape(Points=points,Colour=colour)
            return shape
        else:
            shapes.append(Shape(Points=points,Colour=colour).sort_coordinates())
        
    return shapes

def write_coloured_pieces_to_file(solution,output_file):
    output="<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">"
    for piece in solution:
        string="\n\t<path d=\"M"
        for points in piece.coordinates:
            string+=" {} {} L".format(points.x,points.y)
        output+=string[:-1]+"z\" fill=\"{}\"/>".format(piece.colour)
    output+="\n</svg>"

    output_file.write(output)

def write_to_file(shapes,file): ## for testing
    output_file = open(file,'w')
    write_coloured_pieces_to_file(shapes,output_file)
    output_file.close()

def solve(pieces,shape,attempt=0):

##  checks if the area of the shape and the pieces is the same
    pieces_area=0
    for i in pieces:
        pieces_area+=i.area()
    if pieces_area != shape.area():
        return False

## removes duplicates from list
    def de_dup(lst): 
       out = []
       for i in lst:
           if i not in out:
               out.append(i)
       return out

## returns all possible configurations of the shape
    def get_configs(shape):
        configs=[]
        for i in range(4): 
            configs.append(shape.rotate(i).bring_to())
        for j in range(4): 
            configs.append(configs[j].flip().bring_to())
        return de_dup(configs)

## if the shape if convex, only adds to bottom left corner, otherwise allows for 3 other corners
    if shape.is_convex():
        possible_connection=["SW"]
    else:
        possible_connection=["NW","NE","SW","SE"]

    solution={}
    for p in pieces:
        solution[p.colour]=p

    pieces.sort(key=lambda x: x.area(),reverse=True)
##    pieces.sort(key=lambda x: x.area())
##    shuffle(pieces)
    
    shapes=[shape]
    
    current_try=[deepcopy(pieces),[],None]
    current_try[2]=current_try[0].pop()
    
    configurations=[]
    configurations.append(get_configs(current_try[2]))

    tried=[]

    while shapes != []:
        next_shape=deepcopy(shapes[-1])
        finished=False
        while not finished:

            conf=configurations.pop()

            while conf != []:
                passed=False

                c=conf.pop()

                side=choice(possible_connection)
                connection=next_shape.get_point(side)
                new_piece = c.bring_to(point=connection,side=side)

##  if the new piece is inside the remaining shape, remove the new piece from the shape
                if new_piece.is_in(next_shape):

                    solution[new_piece.colour]=new_piece
                    removed=next_shape.remove(new_piece,connection)
                    
                    next_shape=removed[0]
                    passed=removed[1]

##  if the connecting point is shared by the new piece and the shape, go to next shape
                    if passed:
                        shapes.append(next_shape)
                        configurations.append(conf)

                        tried.append(current_try)
                        current_try=[current_try[0]+current_try[1],[],None]

                        try:
                            current_try[2]=current_try[0].pop()

##  for convex shapes, checks for a false positive, and reruns a maximum of 3 times if not a solution
                        except IndexError:
                            if attempt < 2 and not shape.is_convex():
                                if not is_solution(list(solution.values()),shape):
                                    return solve(pieces,shape,attempt=attempt+1)
                                return list(solution.values())
                            if shape.is_convex() and is_solution(list(solution.values()),shape):
                                return list(solution.values())
                            return False
                            
                        configurations.append(get_configs(current_try[2]))

                        break

##  if fail to find a spot for the piece, add it to tried pieces list and pop the next piece to try
##  if the list it empty, go back and try again from the next configuration in the previous step                    
            else:

                current_try[1].append(current_try[2])

                try:
                    current_try[2]=current_try[0].pop()
                except IndexError:
                    finished=True
                
                configurations.append(get_configs(current_try[2]))

            if passed:
                break
            
        else:
            
            solution[current_try[2].colour]=current_try[2].bring_to(point=Point(400,400))
            current_try=tried.pop()
            conf=configurations.pop()
            if conf == []:
                current_try[1].append(current_try[2])
                current_try[2]=current_try[0].pop()
                configurations.append(get_configs(current_try[2]))
            shapes.pop()
    return False

## checks if the tangram may be a solution to the shape
def is_solution(tangram_in,shape):
    tangram=deepcopy(tangram_in)
    shape_area=shape.area()
    tangram_area=0
    for i in tangram:
        tangram_area+=i.area()
        
        centre=i.centre()
        i.coordinates.sort(key=lambda p: atan2(centre.x-p.x,centre.y-p.y))
    if tangram_area != shape_area:
        return False

    tangram_copy=sum([x.coordinates for x in tangram],[])

    new_shape=None
    end=len(tangram)*10
    count=0
    while tangram != [] and count < end:
        for i in tangram:
            if new_shape is None:
                new_shape = Shape(i.coordinates,"Grey")
                tangram.remove(i)
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
                    index=0
                    if cond1 or cond2:
                        
                        if lines[j].p1 in i.coordinates :
                            new_coord=[[x] for x in new_shape.coordinates]
                            
                            index=i.coordinates.index(connecting_line.p1)
                            
                            rearranged_shape=i.coordinates[index + 1:] + i.coordinates[:index + 1]
                            rearranged_shape.pop()

                            new_coord=sum([rearranged_shape if x==rearranged_shape[0] else [x] for x in new_shape.coordinates],[])
                            new_shape=Shape(new_coord,"Grey")

                            break
                        else:
                            new_coord=[[x] for x in new_shape.coordinates]
                          
                            if cond1:
                                index=i.coordinates.index(connecting_line.p1)
                            elif cond2:
                                index=i.coordinates.index(connecting_line.p1)

                            rearranged_shape=i.coordinates[index + 1:] + i.coordinates[:index + 1]
                            new_coord.insert(new_coord.index([lines[j].p2])+1,rearranged_shape)

                            new_coord=sum(new_coord,[])
                            new_shape=Shape(new_coord,"Grey")
                            break

                tangram.remove(i)

            else:
                count+=1

    if set(shape.coordinates).issubset(set(new_shape.coordinates)) and tangram==[]:
        return True
    elif set(shape.coordinates).issubset(set(tangram_copy)):
        return True

    return False

def testing(coloured_pieces,shape,runs):
    times=[]
    for _ in range(runs):
        start=time.time()
        test_case=solve(coloured_pieces,shape)
        
        if test_case and is_solution(test_case,shape):
            
            times.append(time.time()-start)
            write_to_file(test_case,'test.xml')
        elif test_case:
            write_to_file(test_case,'test2.xml')
        else:
            print('Failed at step ',_)
            break

    if times != []:
        print('average time: ', sum(times)/len(times),len(times),'out of {} finished'.format(runs))
    else:
        print('Failed')


file = open('shape_a_2.xml')
file2 = open('pieces_a.xml')

shape = available_coloured_pieces(file)
coloured_pieces = available_coloured_pieces(file2)

testing(coloured_pieces,shape,100)


