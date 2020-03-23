CC			:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 
LIBS 		:= -lev -lm
	
PREFIX    	:= /usr/local
BINPREFIX 	:= $(PREFIX)/bin

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

DATE_MOD_SRC	:= date_mod.c
DATE_MOD_OBJ 	:= $(DATE_MOD_SRC:.c=.o)
DISK_MOD_SRC	:= disk_usage_mod.c
DISK_MOD_OBJ 	:= $(DISK_MOD_SRC:.c=.o)
VALG 		:= $(wildcard vgcore.*)

all: 		bm_date_mod bm_disk_mod

$(OBJ): 	Makefile 

bm_date_mod: 	$(DATE_MOD_OBJ)
				@mkdir -p build
				$(CC) $(LIBS) -o build/$@ $(DATE_MOD_OBJ)

bm_disk_mod: 	$(DISK_MOD_OBJ)
				@mkdir -p build
				$(CC) $(LIBS) -o build/$@ $(DISK_MOD_OBJ)

install: 
	mkdir -p "$(DESTDIR)$(BINPREFIX)"
	cp -p build/bm_date_mod "$(DESTDIR)$(BINPREFIX)"
	cp -p build/bm_disk_mod "$(DESTDIR)$(BINPREFIX)"

uninstall:
	rm -f "$(DESTDIR)$(BINPREFIX)/bm_date_mod"
	rm -f "$(DESTDIR)$(BINPREFIX)/bm_disk_mod"

clean: 	
	rm -rf build/ $(DISK_MOD_OBJ) $(DATE_MOD_OBJ) $(VALG)


