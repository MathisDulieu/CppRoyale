cmake_minimum_required(VERSION 3.20)
project(CppRoyale CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/msys64/mingw64/lib/cmake/SFML")
find_package(SFML 3 COMPONENTS Graphics Network System Window REQUIRED)

include_directories(shared)

add_executable(server
        server/main.cpp
        server/GameServer.cpp
        server/GameState.cpp
        server/Entity.cpp
        server/Tower.cpp
        server/Lobby.cpp
)
target_link_libraries(server SFML::Network SFML::System)

add_executable(client
        client/main.cpp
        client/GameClient.cpp
        client/Renderer.cpp
        client/Deck.cpp
        client/UIScreen.cpp
)
target_link_libraries(client SFML::Graphics SFML::Network SFML::System SFML::Window)