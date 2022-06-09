function(install_lib LIB_NAME)
    install(
            TARGETS
                ${LIB_NAME}
            EXPORT
                ${LIB_NAME}
            DESTINATION
                lib/${LIB_NAME}-${PROJECT_VERSION}/${CMAKE_BUILD_TYPE}
            PUBLIC_HEADER DESTINATION
                include/${LIB_NAME}-${PROJECT_VERSION}
    )

    install(
            EXPORT
                ${LIB_NAME}
            NAMESPACE
                ${PROJECT_NAME}::
            DESTINATION
                lib/${LIB_NAME}-${PROJECT_VERSION}/${CMAKE_BUILD_TYPE}
    )

    configure_file(${CMAKE_SOURCE_DIR}/cmake/${LIB_NAME}Config-version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/I2CConfig-version.cmake @ONLY)

    install(
            FILES
                ${CMAKE_SOURCE_DIR}/cmake/${LIB_NAME}Config.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config-version.cmake
            DESTINATION
                lib/${LIB_NAME}-${PROJECT_VERSION}
    )

    target_include_directories(${LIB_NAME} PUBLIC $<INSTALL_INTERFACE:include/${LIB_NAME}-${PROJECT_VERSION}>)

endfunction()