#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include <algorithm>
#include "gc_details.h"
#include "gc_iterator.h"

using namespace std;

/// Pointer implements a pointer type that uses garbage collection to release unused memory.
///
/// A Pointer must only be used to point to memory that was dynamically allocated using new.
/// When used to refer to an allocated array, specify the array size.
template<class T, int size = 0>
class Pointer {
 private:
  /// ref_container_ maintains the garbage collection list.
  static list<PtrDetails<T>> ref_container_;

  /// true when first Pointer is created
  static bool first_;

  /// addr_ points to the allocated memory to which this Pointer pointer currently points.
  T *addr_;

  /// is_array_ is true if this Pointer points to an allocated array. It is false otherwise.
  bool is_array_;

  /// If this Pointer is pointing to an allocated array, then array_size_ contains its size.
  unsigned array_size_;

  /// Return an iterator to pointer details in ref_container_.
  typename list<PtrDetails<T> >::iterator FindPtrInfo(T *ptr);

 public:
  /// Empty constructor
  ///
  /// NOTE: templates aren't able to have prototypes with default arguments
  /// this is why constructor is designed like this:
  Pointer() {
    Pointer(NULL);
  }
  Pointer(T *);
  /// Copy constructor.
  Pointer(const Pointer &);
  /// Destructor for Pointer.
  ~Pointer();

  /// Collect garbage. Returns true if at least one object was freed.
  static bool Collect();

  /// Overload assignment of pointer to Pointer.
  T *operator=(T *);
  /// Overload assignment of Pointer to Pointer.
  Pointer &operator=(const Pointer &);
  /// Return a reference to the object pointed to by this Pointer.
  T &operator*() { return *addr_; }
  /// Return the address being pointed to.
  T *operator->() { return addr_; }
  /// Return a reference to the object at the index specified by i.
  T &operator[](int i) { return addr_[i]; }
  /// Conversion function to T *.
  operator T *() { return addr_; }
  /// Return an Iter to the start of the allocated memory.
  Iter<T> Begin() {
    int _size;
    if (is_array_)
      _size = array_size_;
    else
      _size = 1;
    return Iter<T>(addr_, addr_, addr_ + _size);
  }
  /// Return an Iter to one past the end of an allocated array.
  Iter<T> End() {
    int _size;
    if (is_array_)
      _size = array_size_;
    else
      _size = 1;
    return Iter<T>(addr_ + _size, addr_, addr_ + _size);
  }
  /// Return the size of ref_container_ for this type of Pointer.
  static unsigned long RefContainerSize() { return ref_container_.size(); }
  /// A utility function that displays ref_container_.
  static void ShowList();
  /// Clear ref_container_ when program exits.
  static void Shutdown();
};

// STATIC INITIALIZATION
/// Creates storage for the static variables
template<class T, int size>
list<PtrDetails<T>> Pointer<T, size>::ref_container_;
template<class T, int size>
bool Pointer<T, size>::first_ = true;

/// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T, int size>
Pointer<T, size>::Pointer(T *ptr) :addr_(ptr), is_array_(size > 0), array_size_(size) {
  // Register shutdown() as an exit function.
  if (first_)
    atexit(Shutdown);
  first_ = false;

  typename list<PtrDetails<T>>::iterator p = FindPtrInfo(addr_);
  if (p == ref_container_.end()) {
    ref_container_.emplace_front(PtrDetails<T>(addr_, array_size_));
  } else {
    p->ref_count_++;
  }
}

/// Copy constructor.
template<class T, int size>
Pointer<T, size>::Pointer(const Pointer &pointer) {
  addr_ = pointer.addr_;
  is_array_ = pointer.is_array_;
  array_size_ = pointer.array_size_;

  typename list<PtrDetails<T>>::iterator p = FindPtrInfo(addr_);
  assert(p != ref_container_.end());
  p->ref_count_++;
}

/// Destructor for Pointer.
template<class T, int size>
Pointer<T, size>::~Pointer() {
  typename list<PtrDetails<T>>::iterator p = FindPtrInfo(addr_);

  assert(p != ref_container_.end());
  p->ref_count_--;

  Collect();
}

/// Collect garbage. Returns true if at least one object was freed.
template<class T, int size>
bool Pointer<T, size>::Collect() {
  bool deleted = false;

  auto predicate = [&deleted](const PtrDetails<T> &ptr_details) {
    if (ptr_details.ref_count_ == 0) {
      ptr_details.is_array_ ?
      delete[] ptr_details.mem_ptr_ :
      delete ptr_details.mem_ptr_;

      deleted = true;
      return true;
    }

    return false;
  };

  auto start_of_garbage = std::remove_if(ref_container_.begin(), ref_container_.end(), predicate);
  ref_container_.erase(start_of_garbage, ref_container_.end());

  return deleted;
}

/// Overload assignment of pointer to Pointer.
template<class T, int size>
T *Pointer<T, size>::operator=(T *prt) {
  if (addr_ == prt) {
    return *this;
  }

  typename list<PtrDetails<T>>::iterator p = FindPtrInfo(addr_);
  assert(p != ref_container_.end());
  p->ref_count_--;

  Collect();

  addr_ = prt;
  p = FindPtrInfo(addr_);

  if (p == ref_container_.end()) {
    ref_container_.emplace_front(PtrDetails<T>(addr_, size));
  } else {
    p->ref_count_++;
  }

  return *this;
}

/// Overload assignment of Pointer to Pointer.
template<class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(const Pointer &pointer) {
  if (addr_ == pointer.addr_) {
    return *this;
  }

  typename list<PtrDetails<T>>::iterator p = FindPtrInfo(addr_);
  assert(p != ref_container_.end());
  p->ref_count_--;

  Collect();

  addr_ = pointer.addr_;
  p = FindPtrInfo(addr_);
  assert(p != ref_container_.end());
  p->ref_count_++;

  return *this;
}

/// A utility function that displays ref_container_.
template<class T, int size>
void Pointer<T, size>::ShowList() {
  typename list<PtrDetails<T> >::iterator p;
  cout << "ref_container_<" << typeid(T).name() << ", " << size << ">:\n";
  cout << "mem_ptr_ ref_count_ value\n ";
  if (ref_container_.begin() == ref_container_.end()) {
    cout << " Container is empty!\n\n ";
  }
  for (p = ref_container_.begin(); p != ref_container_.end(); p++) {
    cout << "[" << (void *) p->mem_ptr_ << "]"
         << " " << p->ref_count_ << " ";
    if (p->mem_ptr_)
      cout << " " << *p->mem_ptr_;
    else
      cout << "---";
    cout << endl;
  }
  cout << endl;
}

/// Find a pointer in ref_container_.
template<class T, int size>
typename list<PtrDetails<T> >::iterator
Pointer<T, size>::FindPtrInfo(T *ptr) {
  typename list<PtrDetails<T> >::iterator p;
  // Find ptr in ref_container_.
  for (p = ref_container_.begin(); p != ref_container_.end(); p++)
    if (p->mem_ptr_ == ptr)
      return p;
  return p;
}

// Clear ref_container_ when program exits.
template<class T, int size>
void Pointer<T, size>::Shutdown() {
  if (RefContainerSize() == 0)
    return; // list is empty

  for (typename list<PtrDetails<T>>::iterator p = ref_container_.begin(); p != ref_container_.end(); p++) {
    // Set all reference counts to zero
    p->ref_count_ = 0;
  }
  Collect();
}