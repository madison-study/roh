# 컴파일러
CXX = g++
CXXFLAGS = -Wall -std=c++17

# OpenSSL 설치 위치 (Homebrew 기준)
OPENSSL_PATH = /opt/homebrew/opt/openssl@3
INCLUDES = -I$(OPENSSL_PATH)/include
LIBS = -L$(OPENSSL_PATH)/lib -lssl -lcrypto -lresolv

# 소스와 대상
SRC = main.cpp TLSClient.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = main

# 기본 빌드 규칙
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDES) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)
