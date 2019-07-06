using namespace std;

/// Exception thrown when an attempt is made to use an Iter that exceeds the range of
/// the underlying object.
class OutOfRangeExc : exception {
  // Add functionality if needed by your application.
};

/// An iterator-like class for cycling through arrays that are pointed to by GCPtrs.
/// Iter pointers ** do not ** participate in or affect garbage collection. Thus,
/// an Iter pointing to some object does not prevent that object from being recycled.
template<class T>
class Iter {
  /// current pointer value
  T *ptr_;

  /// points to element one past end
  T *end_;

  /// points to start of allocated array
  T *begin_;

  /// length of sequence
  unsigned length_;

 public:
  Iter() {
    ptr_ = end_ = begin_ = NULL;
    length_ = 0;
  }

  Iter(T *p, T *first, T *last) {
    ptr_ = p;
    end_ = last;
    begin_ = first;
    length_ = last - first;
  }

  /// Return length of sequence to which this Iter points.
  unsigned size() { return length_; }

  /// Return value pointed to by ptr. Do not allow out-of-bounds access.
  T &operator*() {
    if ((ptr_ >= end_) || (ptr_ < begin_))
      throw OutOfRangeExc();
    return *ptr_;
  }

  /// Return address contained in ptr. Do not allow out-of-bounds access.
  T *operator->() {
    if ((ptr_ >= end_) || (ptr_ < begin_))
      throw OutOfRangeExc();
    return ptr_;
  }

  /// Prefix ++.
  Iter operator++() {
    ptr_++;
    return *this;
  }

  /// Prefix --.
  Iter operator--() {
    ptr_--;
    return *this;
  }

  /// Postfix ++.
  const Iter operator++(int notused) {
    T *tmp = ptr_;
    ptr_++;
    return Iter<T>(tmp, begin_, end_);
  }

  /// Postfix --.
  const Iter operator--(int notused) {
    T *tmp = ptr_;
    ptr_--;
    return Iter<T>(tmp, begin_, end_);
  }

  /// Return a reference to the object at the specified index. Do not allow
  /// out-of-bounds access.
  T &operator[](int i) {
    if ((i < 0) || (i >= (end_ - begin_)))
      throw OutOfRangeExc();
    return ptr_[i];
  }

  /// Define the relational operators.
  bool operator==(Iter op2) { return ptr_ == op2.ptr_; }
  bool operator!=(Iter op2) { return ptr_ != op2.ptr_; }
  bool operator<(Iter op2) { return ptr_ < op2.ptr_; }
  bool operator<=(Iter op2) { return ptr_ <= op2.ptr_; }
  bool operator>(Iter op2) { return ptr_ > op2.ptr_; }
  bool operator>=(Iter op2) { return ptr_ >= op2.ptr_; }

  /// Subtract an integer from an Iter.
  Iter operator-(int n) {
    ptr_ -= n;
    return *this;
  }

  /// Add an integer to an Iter.
  Iter operator+(int n) {
    ptr_ += n;
    return *this;
  }

  /// Return number of elements between two Iters.
  int operator-(Iter<T> &itr2) { return ptr_ - itr2.ptr_; }
};
