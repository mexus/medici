#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(medichi-test
        main.cpp

	../sources/deck/deck.cpp
	../sources/deck/playing_card.cpp
	../sources/patience/patience.cpp
	../sources/patience/medici.cpp

	../sources/logxx/logxx.cpp
)
