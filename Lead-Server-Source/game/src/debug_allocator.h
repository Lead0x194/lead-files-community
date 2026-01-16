#ifndef _DEBUG_ALLOCATOR_H_
#define _DEBUG_ALLOCATOR_H_

// Debug allocator logic removed; keep allocation macros and get_pointer helpers.

#define M2_NEW new
#define M2_DELETE(p) delete (p)
#define M2_DELETE_EX(p, f, l) delete (p)
#define M2_DELETE_ARRAY(p) delete[] (p)
#define M2_PTR_REF(p) (p)
#define M2_PTR_DEREF(p) (*(p))

// Default get_pointer() free function template.
template<typename T>
T* get_pointer(T* p) {
	return p;
}

#endif // _DEBUG_ALLOCATOR_H_
