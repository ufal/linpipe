%module linpipe_java

// Strings are passed as UTF-8 byte[] and converted on the Java side, because
// the JNI string functions use modified UTF-8, which encodes characters outside
// the BMP as surrogate pairs, see the "Modified UTF-8 Strings" section of
// https://docs.oracle.com/en/java/javase/21/docs/specs/jni/types.html.
// The typemaps must be defined after the standard ones and before linpipe.i.
%include "std_string.i"

%define %linpipe_java_utf8(Type)
%typemap(jni) Type "jbyteArray"
%typemap(jtype) Type "byte[]"
%typemap(jstype) Type "String"
%typemap(javain) Type "$javainput.getBytes(java.nio.charset.StandardCharsets.UTF_8)"
%typemap(javaout) Type { return new String($jnicall, java.nio.charset.StandardCharsets.UTF_8); }
%enddef
%linpipe_java_utf8(std::string)
%linpipe_java_utf8(const std::string&)

%{
static std::string linpipe_java_utf8_in(JNIEnv* jenv, jbyteArray input) {
  std::string result(jenv->GetArrayLength(input), '\0');
  jenv->GetByteArrayRegion(input, 0, jsize(result.size()), reinterpret_cast<jbyte*>(result.data()));
  return result;
}

static jbyteArray linpipe_java_utf8_out(JNIEnv* jenv, std::string_view output) {
  jbyteArray result = jenv->NewByteArray(jsize(output.size()));
  if (result) jenv->SetByteArrayRegion(result, 0, jsize(output.size()), reinterpret_cast<const jbyte*>(output.data()));
  return result;
}
%}
%typemap(in) std::string %{ $1 = linpipe_java_utf8_in(jenv, $input); %}
%typemap(in) const std::string& (std::string temp) %{ temp = linpipe_java_utf8_in(jenv, $input); $1 = &temp; %}
%typemap(out) std::string %{ $result = linpipe_java_utf8_out(jenv, $1); %}
%typemap(out) const std::string& %{ $result = linpipe_java_utf8_out(jenv, *$1); %}

// Java proxies are created in Java, so the factory asks C++ for the name of
// the most-derived listed class and constructs that proxy via reflection.
%define LINPIPE_FACTORY(Base, Types...)
%javamethodmodifiers Base::swig_factory_class_name "protected";
LINPIPE_FACTORY_CLASS_NAME(Base, Types)
%typemap(javaout) Base*, Base& {
    return Base.swig_factory_wrap($jnicall, $owner);
  }
%typemap(javacode) Base %{
  protected static Base swig_factory_wrap(long cPtr, boolean owner) {
    if (cPtr == 0) return null;
    String name = Base.class.getPackage().getName() + "." + new Base(cPtr, false).swig_factory_class_name();
    try {
      return (Base) Class.forName(name).getDeclaredConstructor(long.class, boolean.class).newInstance(cPtr, owner);
    } catch (ReflectiveOperationException e) {
      throw new RuntimeException(e);
    }
  }
%}
%enddef

// LinPipeError is the hand-written LinPipeError.java copied to the package.
#define LINPIPE_RAISE_ERROR(message) \
  do { \
    if (jclass error_class = jenv->FindClass("cz/cuni/mff/ufal/linpipe/LinPipeError")) jenv->ThrowNew(error_class, message.c_str()); \
    return $null; \
  } while (false)

%include "../linpipe/linpipe.i"

%pragma(java) jniclasscode=%{
  static {
    java.io.File libraryFile = new java.io.File(linpipe_java.libraryPath);

    if (libraryFile.exists())
      System.load(libraryFile.getAbsolutePath());
    else
      System.loadLibrary("linpipe_java");
  }
%}

%pragma(java) modulecode=%{
  static String libraryPath;

  static {
    libraryPath = System.mapLibraryName("linpipe_java");
  }

  public static void setLibraryPath(String path) {
    libraryPath = path;
  }
%}
