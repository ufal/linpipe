%module linpipe_java

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
