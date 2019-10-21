set(CMAKE_CXX_STANDARD 17)

file(GLOB source_files
	"*.cpp"
    )


add_library(CounterBase
	${source_files}
	)

install (TARGETS CounterBase DESTINATION bin)
