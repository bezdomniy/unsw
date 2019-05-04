import sys

##  Define point object made from x,y coordinates.
class point(object):
    def __init__(self,x,y):
        self.x = x
        self.y = y

    ## Check if the point is inside a rectangle.
    def is_in_rectangle(self,rect):
        if self.x > rect.left and self.x <= rect.right and self.y >= rect.bottom and self.y <= rect.top:
            return True
        return False

##  Define rectangle object from points.
class rect(object):
    def __init__(self,point_1,point_2):
        self.left = min(point_1.x, point_2.x)
        self.right = max(point_1.x, point_2.x)
        self.bottom = min(point_1.y, point_2.y)
        self.top = max(point_1.y, point_2.y)

    ## Return the coordinates of the rectangle.
    def get_coordinates(self):
        x1 = self.left
        y1 = self.bottom
        x2 = self.right
        y2 = self.top
        return tuple([x1,y1,x2,y2])

    ## Check if another rectangle is entirely contained within this rectangle.
    def is_in(self,rect):
        if rect.left <= self.left:
            if rect.bottom <= self.bottom:
                if rect.right >= self.right:
                    if rect.top >= self.top:
                        return True
        return False

##  Remove rectangles in other rectangles.
def remove_containing(input_set):
    for i in range(len(input_set)):
        for j in range(i+1,len(input_set)):
            if input_set[i].is_in(input_set[j]):
                input_set[i] = blank_rect
    return [x for x in input_set if x != blank_rect]

##  Count the top and bottom edges of the union of the rectangles.
def count_points(input_set,input_perimeter=0,points_counted=set()):
    vertical_perimeter=input_perimeter
    for i in input_set:
        left_bound=i.left
        right_bound=i.right
        for j in range(left_bound,right_bound):
            if input_perimeter == 0:
                check_point=point(j,i.top)
            else:
                check_point=point(j,i.bottom)
            
            for k in [x for x in input_set if x != i]:
                if check_point.is_in_rectangle(k) or (check_point.x,check_point.y) in points_counted:
                    break
            else:
                vertical_perimeter+=1
                points_counted.add((check_point.x,check_point.y))
 
    if input_perimeter == 0:
        return count_points(input_set,vertical_perimeter,points_counted)
    return vertical_perimeter

## Turns a set of rectangles 90 degrees.
def turn_90(input_set):
    new_set = []
    for i in input_set:
        new_set.append(rect(point(i.bottom,i.left),point(i.top,i.right)))
    return new_set

## Prompts the user to input the file name of the rectangle points and checks if the input is valid.
try:
    file = input("Which data file do you want to use? ")
    if file[-4:] != '.txt':
        file+='.txt'
    raw_coordinates = open(file).readlines()
    coordinates = [tuple(map(int,x.split())) for x in raw_coordinates]
    blank_rect = rect(point(0,0),point(0,0))
except:
    print('File does not exist, giving up...')
    sys.exit()

##  Create list of points from coordinates.
points = []
for i in coordinates:
    for j in range(0,len(i),2):
        points.append(point(i[j],i[j+1]))

##  Create list of rectangles from points
rectangles = []
for i in range(0,len(points),2):
    rectangles.append(rect(points[i],points[i+1]))
    
##  Remove any rectangles which are entirely contained within other rectangles
rectangles = remove_containing(rectangles)

## Calculates perimeter of the union of the list of rectangles by counting the
## top and bottom edges, then turning the set 90 degrees, and counting the top
## and bottom edges again
rect_perimeters = count_points(rectangles)+count_points(turn_90(rectangles),points_counted=set())

print('The perimeter is:', rect_perimeters)
