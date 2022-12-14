libdirs { "./extern/SDL2/lib/x64" }
includedirs { "./extern/SDL2/include"}

workspace "HelloWorld"
   configurations { "Debug", "Release" }

project "HelloWorld"
   kind "ConsoleApp"
   language "C++"
   architecture "x64"
   targetdir "bin/%{cfg.buildcfg}"
   links {"SDL2main", "SDL2"}

   files { "**.h", "**.cpp" }


   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"