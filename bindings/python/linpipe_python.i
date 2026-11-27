%module linpipe

// LinPipeError is a Python exception class derived from RuntimeError.
%{
static PyObject* linpipe_error_type = nullptr;
%}
%init %{
  linpipe_error_type = PyErr_NewExceptionWithDoc("linpipe.LinPipeError", "All LinPipe-specific errors are instances of the LinPipeError class.", PyExc_RuntimeError, nullptr);
  if (!linpipe_error_type || PyModule_AddObjectRef(m, "LinPipeError", linpipe_error_type) < 0) return -1;
#ifdef SWIGPYTHON_BUILTIN
  SwigPyBuiltin_AddPublicSymbol(public_interface, "LinPipeError");
#endif
%}
%insert("stub_pyi") %{
class LinPipeError(RuntimeError):
    """All LinPipe-specific errors are instances of the `LinPipeError` class.

    Other errors are reported using the standard Python exceptions; for example,
    invalid argument types raise a [TypeError][] and out-of-range vector indices
    raise an [IndexError][].
    """
%}
// A Python exception already raised (e.g., during iteration) is kept.
#define LINPIPE_RAISE_ERROR(message) do { if (!PyErr_Occurred()) PyErr_SetString(linpipe_error_type, message.c_str()); SWIG_fail; } while (false)

// Strings are passed as std::string_view to avoid copies. Only str is accepted
// (SWIG's typemaps also accept bytes), using its cached UTF-8 representation.
#define LINPIPE_STRING_VIEW std::string_view
%include "std_string_view.i"
%typemap(pytyping) std::string_view "str";
%typemap(typecheck, precedence=SWIG_TYPECHECK_STRINGVIEW) std::string_view "$1 = PyUnicode_Check($input);"
%typemap(in) std::string_view {
  if (!PyUnicode_Check($input)) SWIG_exception_fail(SWIG_TypeError, "in method '$symname', argument $argnum of type '$type'");
  Py_ssize_t size;
  const char* data = PyUnicode_AsUTF8AndSize($input, &size);
  if (!data) SWIG_fail;
  $1 = std::string_view(data, size);
}
%typemap(freearg) std::string_view ""

%include "linpipe_python_vector.i"

%include "../linpipe/linpipe.i"
