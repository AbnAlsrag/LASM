workspace "LASM"
    configurations { "Debug", "Dev", "Release" }

    architecture "x64"
    startproject "LASM"
    compileas "C"
    characterset "MBCS"
    nativewchar "On"
    
    output_dir = "%{cfg.platform}-%{cfg.system}-%{cfg.architecture}"
    
    -- //LASM// --
    project "LASM"
        location "LASM"
        language "C"
        kind "ConsoleApp"
        
        targetdir ("bin/" .. output_dir .. "/%{prj.name}")
        objdir ("bin-int/" .. output_dir .. "/%{prj.name}")
    
        files {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.c",
        }
        
        filter "system:windows"
            systemversion "latest"
            
            defines {
                "LM_PLATFORM_WINDOWS"
            }
 
        filter "configurations:Debug"
            defines { "LM_Debug" }
            boundscheck "On"
            symbols "On"
            
        filter "configurations:Dev"
            defines { "LM_Dev" }
            boundscheck "Off"
            optimize "Debug"

        filter "configurations:Release"
            defines { "LM_Release" }
            boundscheck "Off"
            optimize "Full"
            
    -- //DeLASM// --
    project "DeLASM"
        location "DeLASM"
        language "C"
        kind "ConsoleApp"

        targetdir ("bin/" .. output_dir .. "/%{prj.name}")
        objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

        files {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.c"
        }

        filter "system:windows"
            systemversion "latest"

            defines {
                "LM_PLATFORM_WINDOWS"
            }

        filter "configurations:Debug"
            defines { "LM_Debug" }
            symbols "On"

        filter "configurations:Dev"
            defines { "LM_Dev" }
            optimize "Debug"

        filter "configurations:Release"
            defines { "LM_Release" }
            optimize "Full"