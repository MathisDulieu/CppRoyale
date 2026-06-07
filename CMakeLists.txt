cmake_minimum_required(VERSION 3.20)
project(CppRoyale CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/msys64/mingw64/lib/cmake/SFML")
find_package(SFML 3 COMPONENTS Graphics Network System Window REQUIRED)

include_directories(shared)
include_directories(server)
include_directories(client)

add_executable(server
        server/main.cpp
        server/network/GameServer.cpp
        server/lobby/Lobby.cpp
        server/game/GameState.cpp
        server/game/Entity.cpp
        server/game/Tower.cpp
        server/game/systems/CombatSystem.cpp
        server/game/systems/MovementSystem.cpp
        server/game/systems/WinCondition.cpp
)
target_link_libraries(server SFML::Network SFML::System)

add_executable(client
        client/main.cpp
        client/network/GameClient.cpp
        client/game/Deck.cpp
        client/ui/renderer/Renderer.cpp
        client/ui/screens/UIScreen.cpp
)
target_link_libraries(client SFML::Graphics SFML::Network SFML::System SFML::Window)