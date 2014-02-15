#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4")

add_executable(medichi-test
        main.cpp

	../sources/deck.cpp
	../sources/playing_card.cpp
	../sources/logxx/logxx.cpp
)
