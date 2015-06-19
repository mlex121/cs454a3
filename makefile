.SUFFIXES:

CXX := g++
CXXFLAGS := -std=c++11

debug: CXXFLAGS += -DDEBUG -g -O0

OBJDIR := obj
SRCDIR := src

MKDIR_P = mkdir -p

SERVER := stringServer
SERVER_SRC := stringServer.cpp
SERVER_DEP := stringServer.h
SERVER_LIBS :=

CLIENT := stringClient
CLIENT_SRC := stringClient.cpp
CLIENT_DEP := stringClient.h
CLIENT_LIBS :=

SHARED_SRC := utils.cpp
SHARED_DEP := utils.h
SHARED_LIBS :=

SERVER_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SERVER_SRC))
CLIENT_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(CLIENT_SRC))
SHARED_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SHARED_SRC))

.PHONY: all directories clean nuke

all: directories $(SERVER) $(CLIENT)

debug: all

directories: $(SRCDIR) $(OBJDIR)

$(SRCDIR):
	$(MKDIR_P) $(SRCDIR)

$(OBJDIR):
	$(MKDIR_P) $(OBJDIR)

$(SERVER): $(SERVER_OBJ) $(SHARED_OBJ)
	$(CXX) -o $@ $^ $(SERVER_LIBS)

$(CLIENT): $(CLIENT_OBJ) $(SHARED_OBJ)
	$(CXX) -o $@ $^ $(CLIENT_LIBS)

clean:
	rm -rf $(OBJDIR) $(SERVER) $(CLIENT)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
