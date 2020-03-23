TARGETS= cShell

all: $(TARGETS)

%o: %.c
	gcc -g -c -o  $@ $<
cShell: main.o echo.o functions.o ls.o cd.o pinfo.o history.o exec.o redir.o nightswatch.o -lncurses
	gcc -o cShell main.o echo.o functions.o ls.o cd.o pinfo.o history.o exec.o redir.o nightswatch.o -lncurses

.PHONY: clean
clean:
	rm -f $(TARGETS) *.o
