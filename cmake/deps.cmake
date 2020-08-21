include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_cpm.cmake)
include(${CMAKE_BINARY_DIR}/cmake/CPM_0.27.3.cmake)

CPMAddPackage(
    NAME minctest
    GITHUB_REPOSITORY codeplea/minctest
    GIT_TAG c79fc75a55e826fc5742afc180fc198f81f9f13c
    DOWNLOAD_ONLY
)
