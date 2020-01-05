from conans import ConanFile
from conans import CMake


class NtspConan(ConanFile):
    name = "ntsp"
    version = "1.0"
    license = "BSD"
    author = "Lobanov F.S. ame.fedor.lobanov@gmail.com"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "Self-made smart pointers library, for education purposes"
    topics = ("ntsp", "memory management", "smart pointer")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.h", src="include", dst="include", keep_path=True)
        self.copy("*.lib", "lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["ntsp"]