from conans import ConanFile, CMake

class KrometroConan(ConanFile):
    name = "krometro"
    version = "0.1"
    license = "BSD-new"
    url = "https://github.com/nesono/krometro.git"
    generators = "cmake", "txt", "env"
    default_options = "gtest:shared=False"
    exports = "include/krometro/v1/krometro.hpp", "include/krometro/v1/krometro_main.hpp"
    build_policy = "missing"
    description = "Krometro is a header-only micro benchmark library for C/C++ and is based on the C++11 standard."

    def configure(self):
        if self.scope.dev:
            self.requires("gtest/1.8.0@lasote/stable")

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin") # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib") # From lib to bin

