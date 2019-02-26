# FastVersatileContainer

Hybrid object container with fast insertion, deletion and iteration. It is meant to be an efficient game oriented container *fast and versatile*

This lib is *header only* and has no dependency for your mental sanity

# Usage

## Include *fast_versatile_container.hpp*
```cpp
#include "fast_versatile_container.hpp"
```

## Declare it in a vector like fashion
```cpp
fvc::Container<YourClass> objects;
```

## Add objects in it
```cpp
objects.add(obj);
```

## Iterate over it

```cpp
for (YourClass& obj : objects)
{
  obj.update();
}
```

## Remove objects
```cpp
for (auto it(container->begin()); it != container->end(); ) 
{
  if (it->isDead())
  {
    container->remove(it);
  }
  else 
  {
    ++it;
  }
}
```

# Performance :rocket:

## Size overhead

**16 Bytes** (base class) + **1  Byte per object** + **16 Bytes** *(best case)* or **16 Bytes per object** *(worst case)*

## Operation speed

+ Output of EzBench for **1 000 000 objects** of **40 Bytes** in each container
+ Each one of these test objects has a field with a random value in range *0, 10 000*.
+ Deletion test consists of removing all objects whose value is equal to 0.

```
-----[ Insertion ]-----
List     149.425 ms (100 % -> x1)
Cluster  232.746 ms (155 % -> x1)
Vector   293.807 ms (196 % -> x1)

-----[ Iteration ]-----
Cluster  14.2786 ms (100 % -> x1)
Vector   17.2968 ms (121 % -> x1)
List     66.1563 ms (463 % -> x4)

-----[ Deletion ]-----
Cluster  18.672 ms (100 % -> x1)
List     60.9427 ms (326 % -> x3)
Vector   1850.82 ms (9912 % -> x99)

-----[ Iteration 2 ]-----
Cluster  13.8966 ms (100 % -> x1)
Vector   15.8827 ms (114 % -> x1)
List     63.6529 ms (458 % -> x4)
```         

