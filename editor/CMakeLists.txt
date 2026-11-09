
# 2. Instruct CMake to run moc, uic, and rcc automatically when building
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 3. Find the required Qt6 components (e.g., Core, Widgets, Gui)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
