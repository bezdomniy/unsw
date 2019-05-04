from collections import Iterable

class Ring(list):
    def __init__(self,_data=[]):
        if isinstance(_data,set):
            raise TypeError("'set' object is not subscriptable")
        elif isinstance(_data,int):
            raise TypeError("'int' object is not subscriptable")
        elif isinstance(_data,str):
            raise TypeError("ring indices must be integers or slices, not str")
        elif isinstance(_data,Iterable):
            self._data=_data

        self.start = 0
        self.gen_index=[]
        self.step=1
        self.last = len(self._data)

    def __repr__(self):
        return "Ring({})".format(self._data)

    def __iter__(self):
        return self

    def __next__(self):
        self.start += self.step
        if self._mod(self.start - self.step) in self.gen_index:
            raise StopIteration
        else:
            self.gen_index.append(self._mod(self.start - self.step))
            return self[self.start - self.step]
            
    def __str__(self):
        return str(self._data)

    def __len__(self):
        return len(self._data)

    def _mod(self,i):
        return i%len(self._data)

    def __getitem__(self, key):
        if isinstance(key, slice):
            if not ((isinstance(key.start, int) or key.start is None) and (isinstance(key.stop, int) or key.stop is None) and (isinstance(key.step, int) or key.step is None)):
                raise TypeError("slice indices must be integers or None or have an __index__ method")
            
            if key.start is None:
                start = 0
            else:
                start = key.start

            if key.step is None:
                step = 1
            elif key.step != 0:
                step = key.step
            else:
                if isinstance(self._data,tuple):
                    return tuple()
                else:
                    return []

            if key.stop is None:
                if step >= 0:
                    stop = start - 1
                else:
                    stop = start + 1
            else:
                stop = key.stop
                
            start=self._mod(start)
            stop=self._mod(stop)

            if step > 0:
                if start<=stop:
                    return type(self._data)([self[i] for i in range(start, stop+1, step)])
                return type(self._data)([self[i] for i in range(start, stop+len(self._data)+1, step)])
            else:
                if start>=stop:
                    return type(self._data)([self[i] for i in range(start,stop-1, step)]) ## check this
                return type(self._data)([self[i] for i in range(start+len(self._data),stop-1,  step)])
        else:
            if not (isinstance(key, int)):
                raise TypeError("ring indices must be integers or slices, not str")
            item=self._mod(key)
        return self._data[item]

    def __setitem__(self,key,value):
        if isinstance(key,slice):
            if isinstance(value,int):
                value=[value]
            if key.start is None:
                start = 0
            else:
                start = key.start

            if key.step is None:
                step = 1
            else:
                step = key.step

            if key.stop is None:
                if step >= 0:
                    stop = start - 1
                else:
                    stop = start + 1
            else:
                stop = key.stop
                
            start=self._mod(start)
            stop=self._mod(stop)
            
            if step == 0:
                if start<=stop:
                    if key.stop <= len(self._data):
                        self._data[start:stop+1]=value
                    else:
                        self._data[:key.stop+1]=value
                else:
                    replacement=[i for i in range(start, stop+len(self._data)+1)]
                    for i in replacement:
                        self[i]=None
                    self._data[:stop-1]=value
                    self._data=[x for x in self._data if x is not None]
            elif step > 0:
                if start<stop:
                    indexes=[self[i] for i in range(start, stop,step)]
                else:
                    indexes=[self[i] for i in range(start, stop+len(self._data),step)]

                if len(indexes) != len(value):
                    raise ValueError("attempt to insert sequence of size {} to extended slice of size {}".format(len(value),len(indexes)))

                count=0
                for i in indexes:
                    self._data[i]=[self._data[i],value[count]]
                    count+=1
                if isinstance(self._data[-1],list):
                    self._data.insert(0,self._data[-1].pop())
                    
                self._data=[y for x in self._data for y in (x if isinstance(x, Iterable) else (x,))]

            else:
                if start<=stop:
                    indexes=[self[i] for i in range(start, stop+1,-step)]
                else:
                    indexes=[self[i] for i in range(start, stop+len(self._data)+1,-step)]

                
                if len(indexes) != len(value):
                    raise ValueError("attempt to replace sequence of size {} to extended slice of size {}".format(len(value),len(indexes)))

                count=0
                for i in indexes:
                    self._data[i]=value[count]
                    count+=1
                    
        else:
            self._data[self._mod(key)]=value
