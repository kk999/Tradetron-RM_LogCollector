CC:=clang++
CFLAGS:=-Wall -ansi -std=c++17 -static -g
srcdir:=.
objdir:=../obj
obj:=$(objdir)/main.o $(objdir)/TTUtility.o
exedir:=../bin
exe:=$(exedir)/go

all:$(obj)
	$(CC) -lpthread -o $(exe) $(obj)
$(objdir)/%.o:$(srcdir)/%.cpp
	printf "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY:clean
clean:
	rm -rf $(objdir)/* $(exe)
