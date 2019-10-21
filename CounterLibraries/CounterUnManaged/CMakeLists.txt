set(CMAKE_CXX_STANDARD 17)

file(GLOB source_files
	"*.cpp"
    )

#include_directories($ENV{KSquareLibraries}/KKLineScanner)
#include_directories($ENV{KSquareLibraries}/KKMachineLearning)
include_directories(../CounterBase)

add_library(CounterUnManaged
	${source_files}
	)

install (TARGETS CounterUnManaged DESTINATION bin)
