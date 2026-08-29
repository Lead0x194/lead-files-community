#include "StdAfx.h"
#include "PythonUtils.h"

IPythonExceptionSender * g_pkExceptionSender = NULL;

bool __PyCallClassMemberFunc_ByCString(PyObject* poClass, const char* c_szFunc, PyObject* poArgs, PyObject** poRet);
bool __PyCallClassMemberFunc_ByPyString(PyObject* poClass, PyObject* poFuncName, PyObject* poArgs, PyObject** poRet);
bool __PyCallClassMemberFunc(PyObject* poClass, PyObject* poFunc, PyObject* poArgs, PyObject** poRet);

PyObject * Py_BadArgument()
{
	PyErr_BadArgument();
	return NULL;
}

PyObject * Py_BuildException(const char * c_pszErr, ...)
{
	if (!c_pszErr)
		PyErr_Clear();
	else
	{
		char szErrBuf[512+1];
		va_list args;
		va_start(args, c_pszErr);
		vsnprintf(szErrBuf, sizeof(szErrBuf), c_pszErr, args);
		va_end(args);

		PyErr_SetString(PyExc_RuntimeError, szErrBuf);
	}

	return Py_BuildNone();
	//return NULL;
}

PyObject * Py_BuildNone()
{
	Py_INCREF(Py_None);
	return Py_None;
}

void Py_ReleaseNone()
{
	Py_DECREF(Py_None);
}

bool PyTuple_GetObject(PyObject* poArgs, int pos, PyObject** ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject * poItem = PyTuple_GetItem(poArgs, pos);

	if (!poItem)
		return false;
	
	*ret = poItem;
	return true;
}

bool PyTuple_GetLong(PyObject* poArgs, int pos, long* ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject* poItem = PyTuple_GetItem(poArgs, pos);

	if (!poItem)
		return false;

	if (PyFloat_Check(poItem))
	{
		*ret = (long)PyFloat_AsDouble(poItem);
		return true;
	}

	*ret = PyLong_AsLong(poItem);
	return true;
}

bool PyTuple_GetDouble(PyObject* poArgs, int pos, double* ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject* poItem = PyTuple_GetItem(poArgs, pos);

	if (!poItem)
		return false;

	*ret = PyFloat_AsDouble(poItem);
	return true;
}

bool PyTuple_GetFloat(PyObject* poArgs, int pos, float* ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject * poItem = PyTuple_GetItem(poArgs, pos);

	if (!poItem)
		return false;

	*ret = float(PyFloat_AsDouble(poItem));
	return true;
}

bool PyTuple_GetByte(PyObject* poArgs, int pos, unsigned char* ret)
{
	int val;
	bool result = PyTuple_GetInteger(poArgs,pos,&val);
	*ret = unsigned char(val);
	return result;
}

bool PyTuple_GetInteger(PyObject* poArgs, int pos, unsigned char* ret)
{
	int val;
	bool result = PyTuple_GetInteger(poArgs,pos,&val);
	*ret = unsigned char(val);
	return result;
}

bool PyTuple_GetInteger(PyObject* poArgs, int pos, WORD* ret)
{
	int val;
	bool result = PyTuple_GetInteger(poArgs,pos,&val);
	*ret = WORD(val);
	return result;
}

bool PyTuple_GetInteger(PyObject* poArgs, int pos, int* ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject* poItem = PyTuple_GetItem(poArgs, pos);

	if (!poItem)
		return false;

	if (PyFloat_Check(poItem))
	{
		*ret = (int)PyFloat_AsDouble(poItem);
		return true;
	}

	// LLP64: C long is 32-bit on Win64, so PyLong_AsLong raises OverflowError for any
	// Python int/long > LONG_MAX (e.g. 0x80000000-0xFFFFFFFF window-style flags and ARGB
	// colors, which are normal 32-bit values). Read the low 32 bits with the masking
	// variant (never raises) to match the 32-bit client's behavior for genuine ints.
	*ret = (int)PyLong_AsUnsignedLongMask(poItem);
	return true;
}

bool PyTuple_GetUnsignedLong(PyObject* poArgs, int pos, unsigned long* ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject * poItem = PyTuple_GetItem(poArgs, pos);
	
	if (!poItem)
		return false;

	if (PyFloat_Check(poItem))
	{
		*ret = (unsigned long)(long long)PyFloat_AsDouble(poItem);
		return true;
	}
	
	*ret = PyLong_AsUnsignedLong(poItem);
	return true;
}

bool PyTuple_GetLongLong(PyObject* poArgs, int pos, long long* ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject* poItem = PyTuple_GetItem(poArgs, pos);

	if (!poItem)
		return false;

	if (PyFloat_Check(poItem))
	{
		*ret = (long long)PyFloat_AsDouble(poItem);
		return true;
	}

	*ret = PyLong_AsLongLong(poItem);
	return true;
}

bool PyTuple_GetUnsignedInteger(PyObject* poArgs, int pos, unsigned int* ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject* poItem = PyTuple_GetItem(poArgs, pos);
	
	if (!poItem)
		return false;

	if (PyFloat_Check(poItem))
	{
		*ret = (unsigned int)(long long)PyFloat_AsDouble(poItem);
		return true;
	}
	
	*ret = PyLong_AsUnsignedLong(poItem);
	return true;
}

extern DWORD GetDefaultCodePage();

static const char* __PyUnicode_AsCodePage(PyObject* poUnicode)
{
	static std::string s_astBuffer[16];
	static int s_nBufferIndex = 0;

	Py_ssize_t nWideLen = 0;
	wchar_t* pwszText = PyUnicode_AsWideCharString(poUnicode, &nWideLen);
	if (!pwszText)
		return NULL;

	std::string& rstBuffer = s_astBuffer[s_nBufferIndex];
	s_nBufferIndex = (s_nBufferIndex + 1) % 16;

	int nLen = WideCharToMultiByte(GetDefaultCodePage(), 0, pwszText, (int)nWideLen, NULL, 0, NULL, NULL);
	rstBuffer.assign(nLen, '\0');
	if (nLen > 0)
		WideCharToMultiByte(GetDefaultCodePage(), 0, pwszText, (int)nWideLen, &rstBuffer[0], nLen, NULL, NULL);

	PyMem_Free(pwszText);
	return rstBuffer.c_str();
}

PyObject* PyUnicode_FromCodePage(const char* c_szText)
{
	if (!c_szText || !c_szText[0])
		return PyUnicode_FromString("");

	int nWideLen = MultiByteToWideChar(GetDefaultCodePage(), 0, c_szText, -1, NULL, 0);
	if (nWideLen <= 1)
		return PyUnicode_FromString("");

	std::wstring stWide(nWideLen - 1, L'\0');
	MultiByteToWideChar(GetDefaultCodePage(), 0, c_szText, -1, &stWide[0], nWideLen - 1);
	return PyUnicode_FromWideChar(stWide.c_str(), nWideLen - 1);
}

PyObject* Py_InitModule(const char* c_szName, PyMethodDef* pMethodDef)
{
	PyObject* poModule = PyImport_AddModule(c_szName);
	if (!poModule)
		return NULL;

	if (pMethodDef)
		PyModule_AddFunctions(poModule, pMethodDef);

	return poModule;
}

bool PyTuple_GetString(PyObject* poArgs, int pos, char** ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject* poItem = PyTuple_GetItem(poArgs, pos);

	if (!poItem)
		return false;

	if (!PyUnicode_Check(poItem))
		return false;

	const char* c_szText = __PyUnicode_AsCodePage(poItem);
	if (!c_szText)
		return false;

	*ret = (char*)c_szText;
	return true;
}

bool PyTuple_GetBoolean(PyObject* poArgs, int pos, bool* ret)
{
	if (pos >= PyTuple_Size(poArgs))
		return false;

	PyObject* poItem = PyTuple_GetItem(poArgs, pos);

	if (!poItem)
		return false;

	if (PyFloat_Check(poItem))
	{
		*ret = PyFloat_AsDouble(poItem) != 0.0;
		return true;
	}

	*ret = PyLong_AsLong(poItem) ? true : false;
	return true;
}

PyObject* Py_BuildHandle(const void* ptr)
{
	// "K" packs an unsigned long long (full pointer width on x64) into a Python integer.
	return Py_BuildValue("K", (unsigned long long)(uintptr_t)ptr);
}

bool PyTuple_GetHandle(PyObject* poArgs, int pos, void** ret)
{
	// Read the full 64-bit value back before narrowing to a pointer, so the high half of
	// pointers above 4GB survives the round-trip (the int-based getters would truncate it).
	long long llValue;
	if (!PyTuple_GetLongLong(poArgs, pos, &llValue))
		return false;

	*ret = (void*)(uintptr_t)llValue;
	return true;
}

bool PyCallClassMemberFunc(PyObject* poClass, PyObject* poFunc, PyObject* poArgs)
{
	PyObject* poRet;

	// NOTE: Added NULL check... - [levites]
	if (!poClass)
	{
		Py_XDECREF(poArgs);
		return false;
	}

	if (!__PyCallClassMemberFunc(poClass, poFunc, poArgs, &poRet))
		return false;

	Py_DECREF(poRet);
	return true;
}

bool PyCallClassMemberFunc(PyObject* poClass, const char* c_szFunc, PyObject* poArgs)
{
	PyObject* poRet;

	// NOTE: Added NULL check... - [levites]
	if (!poClass)
	{
		Py_XDECREF(poArgs);
		return false;
	}

	if (!__PyCallClassMemberFunc_ByCString(poClass, c_szFunc, poArgs, &poRet))
		return false;

	Py_DECREF(poRet);
	return true;
}

bool PyCallClassMemberFunc_ByPyString(PyObject* poClass, PyObject* poFuncName, PyObject* poArgs)
{
	PyObject* poRet;

	// NOTE: Added NULL check... - [levites]
	if (!poClass)
	{
		Py_XDECREF(poArgs);
		return false;
	}

	if (!__PyCallClassMemberFunc_ByPyString(poClass, poFuncName, poArgs, &poRet))
		return false;
	
	Py_DECREF(poRet);
	return true;
}

bool PyCallClassMemberFunc(PyObject* poClass, const char* c_szFunc, PyObject* poArgs, bool* pisRet)
{
	PyObject* poRet;

	if (!__PyCallClassMemberFunc_ByCString(poClass, c_szFunc, poArgs, &poRet))
		return false;

	if (PyNumber_Check(poRet))
		*pisRet = (PyLong_AsLong(poRet) != 0);
	else
		*pisRet = true;

	Py_DECREF(poRet);
	return true;
}

bool PyCallClassMemberFunc(PyObject* poClass, const char* c_szFunc, PyObject* poArgs, long * plRetValue)
{
	PyObject* poRet;

	if (!__PyCallClassMemberFunc_ByCString(poClass, c_szFunc, poArgs, &poRet))
		return false;

	if (PyNumber_Check(poRet))
	{
		*plRetValue = PyLong_AsLong(poRet);
		Py_DECREF(poRet);
		return true;
	}

	Py_DECREF(poRet);
	return false;
}

/*
 * Avoid calling this function directly. If you inevitably call it directly, you must call Py_DECREF(poArgs); when false is returned. It does.
 */
bool __PyCallClassMemberFunc_ByCString(PyObject* poClass, const char* c_szFunc, PyObject* poArgs, PyObject** ppoRet)
{
	if (!poClass) 
	{
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject * poFunc = PyObject_GetAttrString(poClass, (char *)c_szFunc);	// New Reference

	if (!poFunc)
	{		
		PyErr_Clear();
		Py_XDECREF(poArgs);
		return false;
	}

	if (!PyCallable_Check(poFunc)) 
	{
		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject * poRet = PyObject_CallObject(poFunc, poArgs);	// New Reference

	if (!poRet)
	{
		if (g_pkExceptionSender)
			g_pkExceptionSender->Clear();

		PyErr_Print();

		if (g_pkExceptionSender)
			g_pkExceptionSender->Send();

		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	*ppoRet = poRet;

	Py_DECREF(poFunc);
	Py_XDECREF(poArgs);
	return true;
}

bool __PyCallClassMemberFunc_ByPyString(PyObject* poClass, PyObject* poFuncName, PyObject* poArgs, PyObject** ppoRet)
{
	if (!poClass) 
	{
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject * poFunc = PyObject_GetAttr(poClass, poFuncName);	// New Reference

	if (!poFunc)
	{		
		PyErr_Clear();
		Py_XDECREF(poArgs);
		return false;
	}

	if (!PyCallable_Check(poFunc)) 
	{
		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject * poRet = PyObject_CallObject(poFunc, poArgs);	// New Reference

	if (!poRet)
	{
		if (g_pkExceptionSender)
			g_pkExceptionSender->Clear();

		PyErr_Print();

		if (g_pkExceptionSender)
			g_pkExceptionSender->Send();

		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	*ppoRet = poRet;

	Py_DECREF(poFunc);
	Py_XDECREF(poArgs);
	return true;
}

bool __PyCallClassMemberFunc(PyObject* poClass, PyObject * poFunc, PyObject* poArgs, PyObject** ppoRet)
{
	if (!poClass) 
	{
		Py_XDECREF(poArgs);
		return false;
	}

	if (!poFunc)
	{		
		PyErr_Clear();
		Py_XDECREF(poArgs);
		return false;
	}

	if (!PyCallable_Check(poFunc)) 
	{
		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	PyObject * poRet = PyObject_CallObject(poFunc, poArgs);	// New Reference

	if (!poRet)
	{
		PyErr_Print();
		Py_DECREF(poFunc);
		Py_XDECREF(poArgs);
		return false;
	}

	*ppoRet = poRet;

	Py_DECREF(poFunc);
	Py_XDECREF(poArgs);
	return true;
}
