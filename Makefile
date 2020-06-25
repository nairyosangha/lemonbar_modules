CC 			:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 
LIBS 		:= -lev -lm
	
PREFIX    	:= /usr/local
BINPREFIX 	:= $(PREFIX)/bin
MODULES 	:= bm_date_mod bm_disk_mod bm_ip_mod

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

DATE_SRC	:= date_mod.c
DISK_SRC	:= disk_usage_mod.c
IP_SRC		:= ip_mod.c
VALG 		:= $(wildcard vgcore.*)

all: 			$(MODULES)

bm_date_mod: 	prep 
	$(CC) $(LIBS) -o build/$@ $(DATE_SRC)

bm_disk_mod: 	prep 
	$(CC) $(LIBS) -o build/$@ $(DISK_SRC)

bm_ip_mod: 		prep 
	$(CC) $(LIBS) -o build/$@ $(IP_SRC)

install: 
	mkdir -p "$(DESTDIR)$(BINPREFIX)"
	for mod in $(MODULES); do install build/$$mod "$(DESTDIR)$(BINPREFIX)"; done

uninstall:
	for mod in $(MODULES); do rm -f "$(DESTDIR)$(BINPREFIX)/$$mod"; done

clean: 	
	rm -rf build/ *.o $(VALG)

prep: 
	@mkdir -p build
