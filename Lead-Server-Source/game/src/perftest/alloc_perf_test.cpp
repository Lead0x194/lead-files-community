#define CASE_DEBUG_NEW_DELETE

#if defined(CASE_RAW_NEW_DELETE)
#elif defined(CASE_RAW_MALLOC_FREE)
#elif defined(CASE_ALLOC_NEW_DELETE)
#define REPLACE_GLOBAL_NEW_DELETE
#elif defined(CASE_ALLOC_MALLOC_FREE)
#define REPLACE_CRT_MALLOC_FREE
#elif defined(CASE_DEBUG_NEW_DELETE)
#define DEBUG_ALLOC
#define REPLACE_GLOBAL_NEW_DELETE
#elif defined(CASE_DEBUG_MALLOC_FREE)
#define DEBUG_ALLOC
#define REPLACE_CRT_MALLOC_FREE
#endif

#include <sys/time.h>

#include <iostream>
#include <string>

#include "../allocator.h" // REPLACE* should be off.

#include "timeval_subtract.c"

using namespace std;

string str_elapsed_secs(struct timeval* from, struct timeval* to) {
	struct timeval tv;
	if (timeval_subtract(&tv, to, from) != 0) {
		return "invalid";
	}
	char buf[256];
	sprintf(buf, "%d.%06d", tv.tv_sec, tv.tv_usec);
	return buf;
}

class Foo {
public:
    Foo() : bar_(0) {
    }
    ~Foo() {
    }
private:
    int bar_;
};

int main(int argc, char* argv[])
{
	const int kCount = 1;

    Allocator::StaticSetUp();

#if defined(CASE_RAW_NEW_DELETE)
	cout << "Raw new/delete " << kCount << " times" << endl;
	Foo* p[kCount];
#elif defined(CASE_RAW_MALLOC_FREE)
	cout << "Raw malloc/free " << kCount << " times" << endl;
	void* p[kCount];
#elif defined(CASE_ALLOC_MALLOC_FREE)
	cout << "Allocator malloc/free " << kCount << " times" << endl;
	void* p[kCount];
#elif defined(CASE_ALLOC_NEW_DELETE)
	cout << "Allocator new/delete " << kCount << " times" << endl;
	Foo* p[kCount];
#elif defined(CASE_DEBUG_MALLOC_FREE)
	cout << "DebugAllocator malloc/free " << kCount << " times" << endl;
	void* p[kCount];
#elif defined(CASE_DEBUG_NEW_DELETE)
	cout << "DebugAllocator new/delete " << kCount << " times" << endl;
	Foo* p[kCount];
#endif

	struct timeval from;
	struct timeval to;

	gettimeofday(&from, NULL);

#if defined(CASE_RAW_NEW_DELETE)
	for (int i = 0; i < kCount; ++i) {
		p[i] = new Foo;
	}
	for (int i = 0; i < kCount; ++i) {
		delete p[i];
	}
#elif defined(CASE_RAW_MALLOC_FREE)
	for (int i = 0; i < kCount; ++i) {
		p[i] = malloc(sizeof(Foo));
	}
	for (int i = 0; i < kCount; ++i) {
		free(p[i]);
	}
#elif defined(CASE_ALLOC_MALLOC_FREE)
	for (int i = 0; i < kCount; ++i) {
		p[i] = M2_MALLOC(sizeof(Foo));
	}
	for (int i = 0; i < kCount; ++i) {
		M2_FREE(p[i]);
	}
#elif defined(CASE_ALLOC_NEW_DELETE)
	for (int i = 0; i < kCount; ++i) {
		p[i] = M2_NEW Foo;
	}
	for (int i = 0; i < kCount; ++i) {
		M2_DELETE(p[i]);
	}
#elif defined(CASE_DEBUG_MALLOC_FREE)
	for (int i = 0; i < kCount; ++i) {
		p[i] = M2_MALLOC(sizeof(Foo));
	}
	for (int i = 0; i < kCount; ++i) {
		M2_FREE(p[i]);
	}
#elif defined(CASE_DEBUG_NEW_DELETE)
	for (int i = 0; i < kCount; ++i) {
		p[i] = M2_NEW Foo;
	}
	for (int i = 0; i < kCount; ++i) {
		M2_DELETE(p[i]);
	}
#endif

	gettimeofday(&to, NULL);

	cout << "Elapsed " << str_elapsed_secs(&from, &to) << " sec(s)" << endl;

    Allocator::StaticTearDown();

	return 0;
}
