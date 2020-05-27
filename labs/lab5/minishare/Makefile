CXXFLAGS =	-O3 -g -mwindows -Wall -fomit-frame-pointer

OBJS =	server.o minishare.o misc.o ipdiag.o files.o config.o gui.o cmdline.o error.o \
auth.o serve.o serve2.o log.o ban.o editfile.o edituser.o $(LANGUAGE).o

LIBS = -lws2_32 -lgdi32 -lcomctl32 -lcomdlg32

LANGUAGE = english
TARGET = minishare.exe

ICONS = res/menu_log.bmp res/menu_config.bmp res/menu_about.bmp res/menu_remove.bmp res/menu_index.bmp res/menu_dir.bmp res/menu_open.bmp res/menu_copy.bmp res/minishare_logo_v2_256.bmp res/list_send.ico

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	
server.o: server.c server.h minishare.h 
	$(CXX) $(CXXFLAGS) server.c -c 

error.o: error.c error.h server.h
	$(CXX) $(CXXFLAGS) error.c -c 

auth.o: auth.c auth.h base64.c
	$(CXX) $(CXXFLAGS) auth.c -c 

ban.o: ban.c ban.h
	$(CXX) $(CXXFLAGS) ban.c -c 
	
log.o: log.c log.h
	$(CXX) $(CXXFLAGS) log.c -c 
	
serve.o: serve.c serve.h
	$(CXX) $(CXXFLAGS) serve.c -c 
		
serve2.o: serve2.c serve2.h
	$(CXX) $(CXXFLAGS) serve2.c -c 
	
minishare.o: minishare.c minishare.h 
	$(CXX) $(CXXFLAGS) minishare.c -c

misc.o: misc.c misc.h minishare.h 
	$(CXX) $(CXXFLAGS) misc.c -c
	
gui.o: gui.cpp gui.h minishare.h 
	$(CXX) $(CXXFLAGS) gui.cpp -c

ipdiag.o: ipdiag.c minishare.h	
	$(CXX) $(CXXFLAGS) ipdiag.c -c

editfile.o: editfile.c minishare.h	
	$(CXX) $(CXXFLAGS) editfile.c -c
	
edituser.o: edituser.c minishare.h	
	$(CXX) $(CXXFLAGS) edituser.c -c
	
files.o: files.c files.h minishare.h 
	$(CXX) $(CXXFLAGS) files.c -c

config.o: config.c config.h minishare.h	
	$(CXX) $(CXXFLAGS) config.c -c

cmdline.o: cmdline.h cmdline.c	
	$(CXX) $(CXXFLAGS) cmdline.c -c

english.o: minishare.rc minishare.h res/english.inc $(ICONS) 
	windres minishare.rc -o english.o --define ENGLISH
	
dutch.o: minishare.rc minishare.h res/dutch.inc $(ICONS) 
	windres minishare.rc -o dutch.o --define DUTCH
	
finnish.o: minishare.rc minishare.h res/finnish.inc $(ICONS) 
	windres minishare.rc -o finnish.o --define FINNISH