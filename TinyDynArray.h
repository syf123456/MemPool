#ifndef __TINYDYNARRAY_H__
#define __TINYDYNARRAY_H__
#include "TinyMemPoolMaroc.h"

SYF_BEGIN
template <class T, int INIT>
class TinyDynArray
{
public:
	enum {SIZE=sizeof(T) };
public:
	TinyDynArray() {
		_mem = _pool;
		_allocated = INIT;
		_size = 0;
	}

	~TinyDynArray() {
		if (_mem != _pool) {
			delete[] _mem;
		}
	}

	void Clear() {
		_size = 0;
	}

	void Push(const T &t) {
		EnsureCapacity(_size + 1);
		_mem[_size++] = t;
	}

	//T* PushArr(T *pArr,const int count) {
	//	EnsureCapacity(_size + count);

	//	for (int i = 0;i < count;++i)
	//		_mem[_size++] = pArr[i];

	//	T* ret = &_mem[_size];
	//	return ret;
	//}

	T Pop() {
		return _mem[--_size];
	}

	void PopArr(int count) {
		POOL_ASSERT(_size >= count);
		_size -= count;
	}

	bool Empty() const {
		return _size == 0;
	}

	T& operator[](int i) {
		POOL_ASSERT(i >= 0 && i < _size);
		return _mem[i];
	}

	const T& operator[](int i) const {
		POOL_ASSERT(i >= 0 && i < _size);
		return _mem[i];
	}

	const T& PeekTop() const {
		POOL_ASSERT(_size > 0);
		return _mem[_size - 1];
	}

	int Size() const {
		return _size;
	}

	int Capacity() const {
		return _allocated;
	}

	const T* Mem() const {
		return _mem;
	}

	T* Mem() {
		return _mem;
	}

private:
	void EnsureCapacity(int cap) {
		if (cap > _allocated) {
			int newAllocated = cap * 2;
			T* newMem = new T[newAllocated];
			memcpy(newMem, _mem, SIZE*_size);	// warning: not using constructors, only works for PODs
			if (_mem != _pool) {
				delete[] _mem;
			}
			_mem = newMem;
			_allocated = newAllocated;
		}
	}

	T*  _mem;
	T   _pool[INIT];
	int _allocated;		// objects allocated
	int _size;			// number objects in use
};
SYF_END

#endif
