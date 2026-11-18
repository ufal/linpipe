%module linpipe

// Allow keyword arguments; std_vector.i is included first, because the
// operator slots of its SwigPyIterator cannot accept keyword arguments.
%include "std_vector.i"
%feature("kwargs");

// LinPipeError is a Python exception class derived from RuntimeError.
%{
static PyObject* linpipe_error_type = nullptr;
%}
%init %{
  linpipe_error_type = PyErr_NewExceptionWithDoc("linpipe.LinPipeError", "An error raised by LinPipe.", PyExc_RuntimeError, nullptr);
  if (!linpipe_error_type || PyModule_AddObjectRef(m, "LinPipeError", linpipe_error_type) < 0) return -1;
#ifdef SWIGPYTHON_BUILTIN
  SwigPyBuiltin_AddPublicSymbol(public_interface, "LinPipeError");
#endif
%}
%insert("stub_pyi") %{
class LinPipeError(RuntimeError):
    """An error raised by LinPipe."""
%}
#define LINPIPE_RAISE_ERROR(message) do { PyErr_SetString(linpipe_error_type, message.c_str()); SWIG_fail; } while (false)

%include "../linpipe/linpipe.i"
