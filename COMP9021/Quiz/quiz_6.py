# Defines two classes, Point() and Disk().
# The latter has an "area" attribute and three methods:
# - change_radius(r)
# - intersects(disk), that returns True or False depending on whether
#   the disk provided as argument intersects the disk object.
# - absord(disk), that returns a new disk object that represents the smallest
#   disk that contains both the disk provided as argument and the disk object.
#
# Written by *** and Eric Martin for COMP9021


from math import pi, hypot, sqrt


class Point():
    def __init__(self, x = 0, y = 0):
        self.x = x
        self.y = y

    def __repr__(self):
        return 'Point({:.2f}, {:.2f})'.format(self.x, self.y)

class Disk():
    def __init__(self,**kwargs):
        self.radius = kwargs.pop('radius',0)
        self.area = pi * self.radius ** 2
        self.point = kwargs.pop('centre',Point())

    def __repr__(self):
        return 'Disk(Point({:.2f}, {:.2f}), {:.2f})'.format(self.point.x, self.point.y, self.radius)

    def change_radius(self,r):
        self.radius = r
        self.area = pi * r ** 2

    def intersects(self,disk):
        return (abs(self.point.x-disk.point.x)**2 + abs(self.point.y-disk.point.y)**2)**(1/2) <= self.radius+disk.radius

    def absorb(self,disk):
        x_dist=self.point.x-disk.point.x
        y_dist=self.point.y-disk.point.y

        dist=sqrt(x_dist**2 + y_dist**2)

        if min(self.radius,disk.radius) + dist < max(self.radius,disk.radius):
            if self.radius > disk.radius:
                big_circle = self
            else:
                big_circle = disk
            return big_circle
        else:
            new_radius = 0.5 * (self.radius+disk.radius+dist)
            x = self.point.x-abs((new_radius - self.radius) * x_dist / dist)
            y = self.point.y-abs((new_radius - self.radius) * y_dist / dist)
            return Disk(centre=Point(x,y),radius=new_radius)

        return Disk(centre = mid_point,radius = max(x_dist,y_dist))
            
            
