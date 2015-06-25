.SUFFIXES:

CXX := g++
CXXFLAGS := -L./lib -I./include -std=c++11

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




SHARED_OBJ := libp.a

SERVER_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SERVER_SRC))
BINDER_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(BINDER_SRC))
LIBRPC_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(LIBRPC_SRC))

.PHONY: all directories clean

all: directories $(SERVER) $(BINDER)

debug: all

directories: $(SRCDIR) $(OBJDIR)

$(SRCDIR):
	$(MKDIR_P) $(SRCDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	    $(CXX) $(CXXFLAGS) $< -c -o $@

$(OBJDIR):
	$(MKDIR_P) $(OBJDIR)

$(INCLUDEDIR):
	$(MKDIR_P) $(INCLUDEDIR)

$(LIBDIR):
	$(MKDIR_P) $(LIBDIR)

$(SERVER): $(SERVER_OBJ) $(LIBRPC)
	$(CXX) $(CXXFLAGS) $(SERVER_LIBS) -o $@ $^ 


$(LIBRPC): $(LIBRPC_OBJ)
	$(AR) $(ARFLAGS) $(LIBRPC) $(LIBRPC_OBJ)

#Untested below this line
$(CLIENT): $(CLIENT_OBJ) $(LIB)
	$(CXX) -L. -o $@ $^ $(CLIENT_LIBS)

clean:
	rm -rf $(OBJDIR) $(SERVER) $(CLIENT)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
