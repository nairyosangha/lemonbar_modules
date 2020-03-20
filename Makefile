CC			:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 
LIBS 		:= -lev
	
PREFIX    	:= /usr/local
BINPREFIX 	:= $(PREFIX)/bin

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

DATE_MOD_SRC	:= date_mod.c
DATE_MOD_OBJ 	:= $(DATE_MOD_SRC:.c=.o)
VALG 		:= $(wildcard vgcore.*)

all: 		date_mod 

$(OBJ): 	Makefile 

date_mod: 	$(DATE_MOD_OBJ)
			@mkdir -p build
			$(CC) $(LIBS) -o build/$@ $(DATE_MOD_OBJ)

install: 
	mkdir -p "$(DESTDIR)$(BINPREFIX)"
	cp -p build/date_mod "$(DESTDIR)$(BINPREFIX)"

uninstall:
	rm -f "$(DESTDIR)$(BINPREFIX)/date_mod"

clean: 	
	rm -rf build/ $(DATE_MOD_OBJ) $(VALG)


