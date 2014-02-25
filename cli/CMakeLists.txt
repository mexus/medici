SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")

add_executable(medici-cli
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
        ../sources/dream_hacking/i-ching/i_ching.cpp

        ../sources/dream_hacking/calculator/calculator.cpp

	../sources/logxx/logxx.cpp
)

target_link_libraries (medici-cli ${CMAKE_THREAD_LIBS_INIT})
