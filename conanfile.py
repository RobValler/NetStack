#/*****************************************************************
# * Copyright (C) 2017 Robert Valler - All rights reserved.
# *
# * This file is part of the project: <insert project name here>
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/

from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy
import os

class NetStackRecipe(ConanFile):
    name = "netstack"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    package_type = "static-library"
    exports_sources = (
        "include/*",
        "lib/*",
    )

    def requirements(self):
        self.requires("gtest/1.17.0")
        self.requires("protobuf/6.32.1")
        self.requires("logger/1.0.1")
        self.requires("openssl/3.6.1")
        self.requires("libssh/0.11.3")
        self.requires("libssh2/1.11.1")
        self.requires("libxml2/2.15.0")

    def layout(self):
        cmake_layout(self)

    def package_info(self):
        self.cpp_info.libs = ["netstack"]
        self.cpp_info.set_property("cmake_target_name", "devfw::netstack")

    def package(self):
        # Headers
        copy(
            self,
            pattern="*.h",
            src=os.path.join(self.export_sources_folder, "include"),
            dst=os.path.join(self.package_folder, "include"),
            keep_path=True,
        )

        # Static libraries
        copy(
            self,
            pattern="*.a",
            src=os.path.join(self.export_sources_folder, "lib"),
            dst=os.path.join(self.package_folder, "lib"),
            keep_path=False,
        )


