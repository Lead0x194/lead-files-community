#include "StdAfx.h"
#include <python/frameobject.h>
#include <python/marshal.h>
#include "../eterPack/EterPackManager.h"

#include "PythonLauncher.h"

CPythonLauncher::CPythonLauncher()
{
	PyConfig config;
	PyConfig_InitIsolatedConfig(&config);
	config.write_bytecode = 0;
	config.module_search_paths_set = 1;

	wchar_t aszPath[_MAX_PATH];
	if (_wfullpath(aszPath, L"lib", _MAX_PATH))
		PyWideStringList_Append(&config.module_search_paths, aszPath);
	if (_wfullpath(aszPath, L".", _MAX_PATH))
		PyWideStringList_Append(&config.module_search_paths, aszPath);

	Py_InitializeFromConfig(&config);
	PyConfig_Clear(&config);
}

CPythonLauncher::~CPythonLauncher()
{
	Clear();
}

void CPythonLauncher::Clear()
{
	Py_Finalize();
}

std::string g_stTraceBuffer[512];
int	g_nCurTraceN = 0;

void Traceback()
{
	std::string str;

	for (int i = 0; i < g_nCurTraceN; ++i)
	{
		str.append(g_stTraceBuffer[i]);
		str.append("\n");
	}
	
	PyObject * exc;
	PyObject * v;
	PyObject * tb;

	PyErr_Fetch(&exc, &v, &tb);

	if (v)
	{
		PyObject * poStr = PyObject_Str(v);
		if (poStr)
		{
			const char * errStr = PyUnicode_AsUTF8(poStr);
			if (errStr)
			{
				str.append("Error: ");
				str.append(errStr);

				Tracef("%s\n", errStr);
			}
			Py_DECREF(poStr);
		}
	}
	Py_XDECREF(exc);
	Py_XDECREF(v);
	Py_XDECREF(tb);
	LogBoxf("Traceback:\n\n%s\n", str.c_str());
}

int TraceFunc(PyObject * obj, PyFrameObject * f, int what, PyObject *arg)
{
	char szTraceBuffer[128];
	PyCodeObject * co;

	switch (what)
	{
		case PyTrace_CALL:
			if (g_nCurTraceN >= 512)
				return 0;

			co = PyFrame_GetCode(f);

			_snprintf_s(szTraceBuffer, sizeof(szTraceBuffer), _TRUNCATE, "Call: File \"%s\", line %d, in %s",
					  PyUnicode_AsUTF8(co->co_filename),
					  PyFrame_GetLineNumber(f),
					  PyUnicode_AsUTF8(co->co_name));

			Py_DECREF(co);
			g_stTraceBuffer[g_nCurTraceN++]=szTraceBuffer;
			break;

		case PyTrace_RETURN:
			if (g_nCurTraceN > 0)
				--g_nCurTraceN;
			break;

		case PyTrace_EXCEPTION:
			if (g_nCurTraceN >= 512)
				return 0;

			co = PyFrame_GetCode(f);

			_snprintf_s(szTraceBuffer, sizeof(szTraceBuffer), _TRUNCATE, "Exception: File \"%s\", line %d, in %s",
					  PyUnicode_AsUTF8(co->co_filename),
					  PyFrame_GetLineNumber(f),
					  PyUnicode_AsUTF8(co->co_name));

			Py_DECREF(co);
			g_stTraceBuffer[g_nCurTraceN++]=szTraceBuffer;

			break;
	}
	return 0;
}

void CPythonLauncher::SetTraceFunc(int (*pFunc)(PyObject * obj, PyFrameObject * f, int what, PyObject *arg))
{
	PyEval_SetTrace(pFunc, NULL);
}

bool CPythonLauncher::Create(const char* c_szProgramName)
{
	NANOBEGIN
#ifdef _DEBUG
	PyEval_SetTrace(TraceFunc, NULL);
#endif
	m_poModule = PyImport_AddModule((char *) "__main__");

	if (!m_poModule)
		return false;

	m_poDic = PyModule_GetDict(m_poModule);

    PyObject * builtins = PyImport_ImportModule("builtins");
	PyModule_AddIntConstant(builtins, "TRUE", 1);
	PyModule_AddIntConstant(builtins, "FALSE", 0);
    PyDict_SetItemString(m_poDic, "__builtins__", builtins);
	Py_DECREF(builtins);

	if (!RunLine("import __main__"))
		return false;
	
	if (!RunLine("import sys"))
		return false;

	NANOEND
	return true;
}

bool CPythonLauncher::RunCompiledFile(const char* c_szFileName)
{
	NANOBEGIN
	FILE * fp = NULL;
	if (fopen_s(&fp, c_szFileName, "rb") != 0 || !fp)
		return false;

	PyObject *co;
	PyObject *v;
	long magic;

	magic = PyMarshal_ReadLongFromFile(fp);

	if (magic != PyImport_GetMagicNumber())
	{
		PyErr_SetString(PyExc_RuntimeError, "Bad magic number in .pyc file");
		fclose(fp);
		return false;
	}

	PyMarshal_ReadLongFromFile(fp);
	PyMarshal_ReadLongFromFile(fp);
	PyMarshal_ReadLongFromFile(fp);
	co = PyMarshal_ReadLastObjectFromFile(fp);

	fclose(fp);

	if (!co || !PyCode_Check(co))
	{
		Py_XDECREF(co);
		PyErr_SetString(PyExc_RuntimeError, "Bad code object in .pyc file");
		return false;
	}

	v = PyEval_EvalCode(co, m_poDic, m_poDic);
	Py_DECREF(co);
	if (!v)
	{
		Traceback();
		return false;
	}

	Py_DECREF(v);

	NANOEND
	return true;
}


bool CPythonLauncher::RunMemoryTextFile(const char* c_szFileName, UINT uFileSize, const VOID* c_pvFileData)
{
	NANOBEGIN
	const CHAR* c_pcFileData=(const CHAR*)c_pvFileData;

	std::string stConvFileData;
	stConvFileData.reserve(uFileSize);
	stConvFileData+="exec(compile('''";

	// ConvertPythonTextFormat
	{
		for (UINT i=0; i<uFileSize; ++i)
		{
			if (c_pcFileData[i]!=13)
				stConvFileData+=c_pcFileData[i];
		}
	}

	stConvFileData+= "''', ";
	stConvFileData+= "'";
	stConvFileData+= c_szFileName;
	stConvFileData+= "', ";
	stConvFileData+= "'exec'))";

	const CHAR* c_pcConvFileData=stConvFileData.c_str();
	NANOEND
	return RunLine(c_pcConvFileData);
}

bool CPythonLauncher::RunFile(const char* c_szFileName)
{
	char* acBufData=NULL;
	DWORD dwBufSize=0;
	
	{
		CMappedFile file;
		const VOID* pvData;
		CEterPackManager::Instance().Get(file, c_szFileName, &pvData);
		
		dwBufSize=file.Size();
		if (dwBufSize==0)
			return false;
		
		acBufData=new char[dwBufSize];
		memcpy(acBufData, pvData, dwBufSize);	
	}

	bool ret=false;
	
	ret=RunMemoryTextFile(c_szFileName, dwBufSize, acBufData);

	delete [] acBufData;
	
	return ret;
}

bool CPythonLauncher::RunLine(const char* c_szSrc)
{
	PyObject * v = PyRun_String((char *) c_szSrc, Py_file_input, m_poDic, m_poDic);

	if (!v)
	{
		Traceback();
		return false;
	}

	Py_DECREF(v);
	return true;
}

const char* CPythonLauncher::GetError()
{
	static std::string s_stError;

	PyObject* exc;
	PyObject* v;
	PyObject* tb;

	PyErr_Fetch(&exc, &v, &tb);

	s_stError.clear();

	if (v)
	{
		PyObject* poStr = PyObject_Str(v);
		if (poStr)
		{
			const char* errStr = PyUnicode_AsUTF8(poStr);
			if (errStr)
				s_stError = errStr;
			Py_DECREF(poStr);
		}
	}

	Py_XDECREF(exc);
	Py_XDECREF(v);
	Py_XDECREF(tb);

	return s_stError.c_str();
}
