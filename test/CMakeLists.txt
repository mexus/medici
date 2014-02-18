SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Og")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")

add_executable(medichi-test
        main.cpp

	../sources/deck/deck.cpp
	../sources/deck/playing_card.cpp
	../sources/patience/patience.cpp
	../sources/patience/medici.cpp

        ../sources/dream_hacking/card_selector.cpp

        ../sources/dream_hacking/range_selectors/range_selector.cpp
        ../sources/dream_hacking/range_selectors/existential_range_selector.cpp
        ../sources/dream_hacking/range_selectors/universal_range_selector.cpp
        ../sources/dream_hacking/range_selectors/complex_range_selector.cpp

        ../sources/dream_hacking/calculator.cpp

	../sources/logxx/logxx.cpp
)
