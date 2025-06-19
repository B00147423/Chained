vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO CedricGuillemet/ImGuizmo
    REF master
    SHA512 0
)

file(INSTALL ${SOURCE_PATH}/ImGuizmo.h DESTINATION ${CURRENT_PACKAGES_DIR}/include)
file(INSTALL ${SOURCE_PATH}/ImGuizmo.cpp DESTINATION ${CURRENT_PACKAGES_DIR}/src)
