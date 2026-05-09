cmake_minimum_required(VERSION 3.16)
project(IceFairyBulletHell)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

# SFML configuration
set(SFML_DIR "D:/SFML-2.6.2")
set(SFML_INCLUDE_DIRS "D:/SFML-2.6.2/include")
set(SFML_LIBRARY_DIRS "D:/SFML-2.6.2/lib")

# Source files
set(SOURCES
    main.cpp
    Player.cpp
    PlayerBullet.cpp
    BombEffect.cpp
    GameUI.cpp
    DifficultySelect.cpp
    SaveSelect.cpp
    SoundManager.cpp
    Boss.cpp
    EnemyBullet.cpp
)

# Header files
set(HEADERS
    Animation.h
    Player.h
    PlayerBullet.h
    BombEffect.h
    GameUI.h
    DifficultySelect.h
    SaveSelect.h
    SoundManager.h
    Boss.h
    EnemyBullet.h
)

# Create executable
add_executable(game WIN32 ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(game PRIVATE
    ${SFML_INCLUDE_DIRS}
)

# Library directories
target_link_directories(game PRIVATE
    ${SFML_LIBRARY_DIRS}
)

# Link SFML libraries
target_link_libraries(game
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system
    sfml-network
)

# Windows specific configuration
if(WIN32)
    target_link_libraries(game ws2_32)
endif()

# Print configuration info
message(STATUS "SFML include: ${SFML_INCLUDE_DIRS}")
message(STATUS "SFML libraries: ${SFML_LIBRARY_DIRS}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")