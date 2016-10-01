#ifndef __TINYMEMPOOLIMP__
#define __TINYMEMPOOLIMP__
#include "TinyMemPoolMaroc.h"
#include "TinyConstructHelp.h"
#include "TinyDynArray.h"

SYF_BEGIN
template<int SIZE,int BLOCKSIZE=10>
class TinyMemPool
{
public:
	TinyMemPool() :_currentAllocs(0),_nUntracked(0)
	{
		initWithBlock(BLOCKSIZE);
	}
	~TinyMemPool() {
		// Delete the blocks.
		for (int i = 0; i < _blockPtrs.Size(); ++i) {
			delete _blockPtrs[i];
		}
	}

	int ItemSize() const {
		return SIZE;
	}
	int CurrentAllocs() const {
		return _currentAllocs;
	}
	int MaxAllocs()const
	{
		return _maxAllocs;
	}

	void* Alloc() {
		if (!_root) {
			// Need a new block.
			Block* block = new Block();
			_blockPtrs.Push(block);

			for (int i = 0; i < COUNT - 1; ++i) {
				block->chunk[i].next = &block->chunk[i + 1];
			}
			block->chunk[COUNT - 1].next = 0;
			_root = block->chunk;

			//refresh max allocates
			_maxAllocs += COUNT;
		}
		void* result = _root->mem;
		_root = _root->next;

		++_currentAllocs;
		if (_currentAllocs > _maxAllocs) {
			_maxAllocs = _currentAllocs;
		}
		_nUntracked++;
		return result;
	}
	void Free(void* mem) {
		if (!mem) {
			return;
		}
		--_currentAllocs;
		Chunk* chunk = (Chunk*)mem;
#ifdef TINY_DEBUG
		memset(chunk, 0xfe, sizeof(Chunk));
#endif
		chunk->next = _root;
		_root = chunk;
	}
	void Trace(const char* name) {
		printf("Mempool %s watermark=%d [%dk] current=%d size=%d blocks=%d\n",
			name, _maxAllocs, _maxAllocs*SIZE / 1024, _currentAllocs, SIZE, _nAllocs, _blockPtrs.Size());
	}

	void SetTracked() {
		_nUntracked--;
	}

	int Untracked() const {
		return _nUntracked;
	}

	// This number is perf sensitive. 4k seems like a good tradeoff on my machine.
	// The test file is large, 170k.
	// Release:		VS2010 gcc(no opt)
	//		1k:		4000
	//		2k:		4000
	//		4k:		3900	21000
	//		16k:	5200
	//		32k:	4300
	//		64k:	4000	21000
	enum { COUNT = (4 * 1024) / SIZE }; // Some compilers do not accept to use COUNT in private part if COUNT is private

protected:
	//init with blockSize
	void initWithBlock(const int blocks)
	{
		if (blocks <= 0)
		{
			_root = 0;
			return;
		}

		//push blocks
		for (int i=0;i<blocks;++i)
			_blockPtrs.Push(new Block());

		POOL_ASSERT(_blockPtrs[0]);
		_root = _blockPtrs[0]->chunk;

		for (int i = 0;i < blocks;++i)
		{
			POOL_ASSERT(_blockPtrs[i]);
			for (int j = 0;j < COUNT - 1;++j)
				_blockPtrs[i]->chunk[j].next = &_blockPtrs[i]->chunk[j + 1];

			if (blocks - 1 == i)
				_blockPtrs[i]->chunk[COUNT - 1].next = 0;
			else
				_blockPtrs[i]->chunk[COUNT - 1].next = _blockPtrs[i + 1]->chunk;
		}

		_maxAllocs = blocks * COUNT;
	}
private:
	union Chunk {
		Chunk*  next;
		char    mem[SIZE];
	};
	struct Block {
		Chunk chunk[COUNT];
	};
	TinyDynArray< Block*, BLOCKSIZE > _blockPtrs;
	Chunk* _root;

	int _currentAllocs;
	int _maxAllocs;
	int _nUntracked;

	template<typename Type,int,typename AllocateType>
	friend class TinyMemCreator;
};


template<typename Type, int BlockSize=10,typename AllocateType= TinyMemPool<sizeof(Type), BlockSize> >
class TinyMemCreator
{
public:
	TinyMemCreator():_allocate(new AllocateType())
	{
	}

	~TinyMemCreator()
	{
		if (_allocate)
		{
			delete _allocate;
			_allocate = nullptr;
		}
	}

	Type* Alloc()
	{
		void *pMem=_allocate->Alloc();
		return TinyConstructHelp<Type>::construct(pMem);
	}

	Type* Alloc(const Type &t)
	{
		void *pMem = _allocate->Alloc();
		return TinyConstructHelp<Type>::construct(t);
	}

	template<typename... Args>
	Type* Alloc(const Args&... args)
	{
		void *pMem = _allocate->Alloc();
		return TinyConstructHelp<Type>::construct(pMem,args...);
	}

	void Free(Type *p)
	{
		TinyConstructHelp<Type>::destroy(p);
		_allocate->Free(p);
	}

	int Capacity()const
	{
		return _allocate->MaxAllocs();
	}
	int Count()const
	{
		return _allocate->CurrentAllocs();
	}
private:
	AllocateType *_allocate;
};
SYF_END
#endif
