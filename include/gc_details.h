/// This class defines an element that is stored in the garbage collection information list.
template<class T>
class PtrDetails {
 public:
  /// Here, mPtr points to the allocated memory. If this is an array, then size specifies
  /// the size of the array.
  explicit PtrDetails(T *pointer, const int array_size)
      : mem_ptr_(pointer), array_size_(array_size), is_array_(array_size != 0), ref_count_(1) {}

  /// current reference count
  unsigned ref_count_;

  /// pointer to allocated memory
  T *mem_ptr_;

  /// is_array_ is true if mem_ptr_ points to an allocated array. It is false otherwise.
  bool is_array_;

  /// If mem_ptr_ is pointing to an allocated array, then array_size_ contains its size
  unsigned array_size_;
};

/// Overloading operator== allows two class objects to be compared.
/// This is needed by the STL list class.
template<class T>
bool operator==(const PtrDetails<T> &ob1, const PtrDetails<T> &ob2) {
  return ob1.mem_ptr_ == ob2.mem_ptr_;
}