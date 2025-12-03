#ifndef ED_HEAP_H
#define ED_HEAP_H

template <typename T, typename... ARG>
T *ed_heap_alloc(ARG &&...args) {
  void *block = eg_heap_alloc(sizeof(T), 1, 0);

  return new (block) T(std::forward<ARG>(args)...);
}

template <typename T>
void ed_heap_free(T *block) {
  block->~T();

  eg_heap_free(block);
}

#endif // ED_HEAP_H
