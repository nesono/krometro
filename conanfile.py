from conans import ConanFile, CMake

class KrometroConan(ConanFile):
   name = "krometro"
   version = "0.1"
   license = "BSD-new"
   url = "https://github.com/nesono/krometro.git"
   settings = "os", "compiler", "build_type", "arch"
   requires = "gtest/1.8.0@lasote/stable"
   generators = "cmake"
   default_options = "gtest:shared=False"

   def imports(self):
      self.copy("*.h", dst="include/krometro/v1", src="krometro/v1")
      self.copy("*.cpp", dst="test", src=".")

   def build(self):
      cmake = CMake(self.settings)
      self.run('cmake "%s" %s' % (self.conanfile_directory, cmake.command_line))
      self.run('cmake --build . %s' % cmake.build_config)
