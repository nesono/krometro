from conans import ConanFile, CMake

class KrometroConan(ConanFile):
    name = "krometro"
    version = "0.1"
    license = "BSD-new"
    url = "https://github.com/nesono/krometro.git"
    requires = "gtest/1.8.0@lasote/stable"
    generators = "cmake", "txt", "env"
    default_options = "gtest:shared=False"
    exports = "include/krometro/v1/krometro.h"
    build_policy = "missing"

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin") # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib") # From lib to bin

