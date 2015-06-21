.SUFFIXES:

CXX := g++
CXXFLAGS := -L. -std=c++11

debug: CXXFLAGS += -DDEBUG -g -O0

AR := ar
ARFLAGS := rcsu

OBJDIR := obj
SRCDIR := src
INCLUDEDIR := include
LIBDIR := lib

MKDIR_P = mkdir -p

SERVER := server
SERVER_SRC := server.cpp
SERVER_DEP := server.h
SERVER_LIBS := -lrpc

BINDER := binder
BINDER_SRC := binder.cpp
BINDER_DEP := binder.h
BINDER_LIBS := -lrpc

CLIENT := client
CLIENT_SRC := client.cpp
CLIENT_DEP := client.h
CLIENT_LIBS := -lrpc

LIBRPC := $(LIBDIR)/librpc.a
LIBRPC_SRC := rpc.cpp
LIBRPC_DEP := rpc.h rpc_private.h
LIBRPC_LIBS :=

SERVER_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SERVER_SRC))
BINDER_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(BINDER_SRC))
LIBRPC_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(LIBRPC_SRC))

.PHONY: all directories clean

all: directories $(SERVER) $(BINDER)

debug: all

directories: $(SRCDIR) $(OBJDIR)

$(SRCDIR):
	$(MKDIR_P) $(SRCDIR)

$(OBJDIR):
	$(MKDIR_P) $(OBJDIR)

$(INCLUDEDIR):
	$(MKDIR_P) $(INCLUDEDIR)

$(LIBDIR):
	$(MKDIR_P) $(LIBDIR)

$(SERVER): $(SERVER_OBJ) $(SHARED_OBJ)
	$(CXX) -L. -o $@ $^ $(SERVER_LIBS)

$(CLIENT): $(CLIENT_OBJ) $(LIB)
	$(CXX) -L. -o $@ $^ $(CLIENT_LIBS)

$(LIBRPC): $(LIBRPC_OBJ)
	$(AR) $(ARFLAGS) $(LIBRPC) $(LIBRPC_OBJ)

clean:
	rm -rf $(OBJDIR) $(SERVER) $(CLIENT)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
