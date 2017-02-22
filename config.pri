# Comment/Uncomment to use static or shared lib
CONFIG += static

CONFIG -= debug_and_release

CORE_LIB_NAME = qhttpengine
CONFIG(debug, debug|release) {
	TARGET = $${TARGET}d # Append a "d" suffix on debug libs.
	CORE_LIB_NAME = $${CORE_LIB_NAME}d
}

contains(QMAKE_CC, cl) {
	CORE_LIB_FILE = $${CORE_LIB_NAME}.lib
} else {
	static: CORE_LIB_FILE = lib$${CORE_LIB_NAME}.a
	!static: CORE_LIB_FILE = lib$${CORE_LIB_NAME}.so
	QMAKE_CXXFLAGS += --std=c++11
}

