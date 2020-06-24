CC			:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 
LIBS 		:= -lev -lm
	
PREFIX    	:= /usr/local
BINPREFIX 	:= $(PREFIX)/bin
MODULES 	:= bm_date_mod bm_disk_mod bm_ip_mod

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

DATE_MOD_SRC	:= date_mod.c
DATE_MOD_OBJ 	:= $(DATE_MOD_SRC:.c=.o)
DISK_MOD_SRC	:= disk_usage_mod.c
DISK_MOD_OBJ 	:= $(DISK_MOD_SRC:.c=.o)
IP_MOD_SRC		:= ip_mod.c
IP_MOD_OBJ 		:= $(IP_MOD_SRC:.c=.o)
VALG 			:= $(wildcard vgcore.*)

all: 		$(MODULES)

$(OBJ): 	Makefile 

bm_date_mod: 	$(DATE_MOD_OBJ)
				@mkdir -p build
				$(CC) $(LIBS) -o build/$@ $(DATE_MOD_OBJ)

bm_ip_mod: 		$(IP_MOD_OBJ)
				@mkdir -p build
				$(CC) $(LIBS) -o build/$@ $(IP_MOD_OBJ)

bm_disk_mod: 	$(DISK_MOD_OBJ)
				@mkdir -p build
				$(CC) $(LIBS) -o build/$@ $(DISK_MOD_OBJ)

install: 
	mkdir -p "$(DESTDIR)$(BINPREFIX)"
	for dir in $(MODULES); do install build/$$dir "$(DESTDIR)$(BINPREFIX)"; done

uninstall:
	for dir in $(MODULES); do rm -f "$(DESTDIR)$(BINPREFIX)/$$dir"; done

clean: 	
	rm -rf build/ $(DISK_MOD_OBJ) $(DATE_MOD_OBJ) $(IP_MOD_OBJ) $(VALG)


