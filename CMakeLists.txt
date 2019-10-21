cmake_minimum_required(VERSION 3.0.0)
#roject(KSquareUtilities VERSION 0.1.0)

#nable_testing()

set(CMAKE_CXX_STANDARD 17)

include_directories($ENV{KSquareLibraries}/KKBase)
include_directories($ENV{KSquareLibraries}/KKLineScanner)
include_directories($ENV{KSquareLibraries}/KKMachineLearning)
include_directories($ENV{PUREGEV_ROOT}/Includes)

subdirs(CounterLibraries)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
