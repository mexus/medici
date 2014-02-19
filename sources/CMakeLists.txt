SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")

add_executable(medichi
        main.cpp

	deck/deck.cpp
	deck/playing_card.cpp
	patience/patience.cpp
	patience/medici.cpp

        dream_hacking/card_selector.cpp

        dream_hacking/range_selectors/range_selector.cpp
        dream_hacking/range_selectors/existential_range_selector.cpp
        dream_hacking/range_selectors/universal_range_selector.cpp
        dream_hacking/range_selectors/complex_range_selector.cpp

        dream_hacking/calculator.cpp

	logxx/logxx.cpp
)

target_link_libraries (medichi ${CMAKE_THREAD_LIBS_INIT})