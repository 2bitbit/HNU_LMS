file(STRINGS "D:/Workspace/Vscode/Repos/H_LMS/build/dependencies.txt" DLL_LIST)
            foreach(DLL_NAME IN LISTS DLL_LIST)
                string(REGEX REPLACE "^.*DLL Name: " "" DLL_NAME "")
                find_file(DLL_PATH "" 
                    PATHS  
                    NO_DEFAULT_PATH)
                if(DLL_PATH)
                    file(COPY "" DESTINATION "$<TARGET_FILE_DIR:H_LMS>")
                endif()
            endforeach()