# ObjectPool

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
So fast :rocket:
