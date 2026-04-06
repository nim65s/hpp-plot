{
  description = "Graphical utilities for constraint graphs in hpp-manipulation";

  inputs = {
    gepetto.url = "github:gepetto/nix";
    flake-parts.follows = "gepetto/flake-parts";
    systems.follows = "gepetto/systems";
  };

  outputs =
    inputs:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } (
      { lib, ... }:
      {
        systems = import inputs.systems;
        imports = [
          inputs.gepetto.flakeModule
          {
            flakoboros.overrideAttrs.hpp-plot = _: {
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
            };
          }
        ];
      }
    );
}
