#include "pch.h"


#include "memory/VaribleIndexAllocator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GPUObjectsTests
{
	TEST_CLASS(VaribleSizeAllocatorTests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			VaribleIndexAllocator allocator(30);

			allocator.alloc(10);
			allocator.alloc(10);
			allocator.alloc(10);

			size_t& asize = allocator.allocatedSize;

			Assert::AreEqual(size_t(30), asize);

			allocator.free(0, 10);
			allocator.free(20, 10);
			allocator.free(10, 10);

			Assert::AreEqual(size_t(0), asize);

			allocator.alloc(28);
			allocator.alloc(2);

			Assert::AreEqual(size_t(10), asize);

			Assert::AreEqual(size_t(0), allocator.freeSpaces.size());
			Assert::AreEqual(size_t(0), allocator.freeSpaces_beginnings.size());
			Assert::AreEqual(size_t(0), allocator.freeSpaces_ends.size());
			Assert::AreEqual(size_t(0), allocator.noLongerFreeSpaceHitList.size());
		}
	};
}
