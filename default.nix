{
  lib,
  stdenv,
  cmake,
  gepetto-viewer-corba,
  hpp-manipulation-corba,
  pkg-config,
  libsForQt5,
}:

stdenv.mkDerivation {
  pname = "hpp-plot";
  version = "5.0.0";

  src = lib.fileset.toSource {
    root = ./.;
    fileset = lib.fileset.unions [
      ./bin
      ./cmake_modules
      ./CMakeLists.txt
      ./doc
      ./include
      ./package.xml
      ./plugins
      ./src
    ];
  };

  strictDeps = true;

  nativeBuildInputs = [
    cmake
    libsForQt5.wrapQtAppsHook
    pkg-config
  ];
  buildInputs = [ libsForQt5.qtbase ];
  propagatedBuildInputs = [
    gepetto-viewer-corba
    hpp-manipulation-corba
  ];

  doCheck = true;

  meta = {
    description = "Graphical utilities for constraint graphs in hpp-manipulation";
    homepage = "https://github.com/humanoid-path-planner/hpp-plot";
    license = lib.licenses.bsd2;
    maintainers = [ lib.maintainers.nim65s ];
  };
}
