
PROJECT   = Headz
VERSION   = 1.18
AUTHOR    = Jeff  

############################################################################################################################

PRE_DEFINE = #NDEBUG

############################################################################################################################

INC_PATH = ../src/ ../tst/ ../egg/
INC_FILE = 		    

SRC1_PATH = ../src/
SRC1_FILE = listz.c jeffz.c bitopz.c bitmapz.c slabz.c slakz.c rankz.c hashz.c dbugz.c \
            bintreez.c avltreez.c rbtreez.c rxtreez.c sptreez.c treapz.c sbtreez.c     \
            priheapz.c multreez.c rxstreez.c octreez.c hstreez.c atomz.c timez.c

SRC2_PATH = ../egg/
SRC2_FILE = egg_main.cpp dbugz_eg.cpp jeffz_eg.cpp 

SRC3_PATH = ../tst/
SRC3_FILE = avltreez_ut.cpp  jeffz_ut.cpp     rbtreez_ut.cpp   slakz_ut.cpp     \
            bintreez_ut.cpp  listz_ut.cpp     rxstreez_ut.cpp  sptreez_ut.cpp   \
            bitmapz_ut.cpp   multreez_ut.cpp  rxtreez_ut.cpp   treapz_ut.cpp    \
            bitopz_ut.cpp    priheapz_ut.cpp  sbtreez_ut.cpp   utilz_ut.cpp     \
            hashz_ut.cpp     rankz_ut.cpp     slabz_ut.cpp     dbugz_ut.cpp     \
            h2unit.cpp       ut_stub.cpp      hstreez_ut.cpp 

            
############################################################################################################################

OBJ_PATH = ../obj/
BIN_PATH = ../bin/

LIB_PATH = $(BIN_PATH)
LIB_FILE = m pthread

############################################################################################################################

ifeq ($(wildcard $(OBJ_PATH)),)
 $(shell mkdir -p $(OBJ_PATH))
endif
ifeq ($(wildcard $(OBJ_PATH)o1/),)
 $(shell mkdir -p $(OBJ_PATH)o1/)
endif
ifeq ($(wildcard $(OBJ_PATH)o2/),)
 $(shell mkdir -p $(OBJ_PATH)o2/)
endif
ifeq ($(wildcard $(OBJ_PATH)o3/),)
 $(shell mkdir -p $(OBJ_PATH)o3/)
endif
ifeq ($(wildcard $(OBJ_PATH)o4/),)
 $(shell mkdir -p $(OBJ_PATH)o4/)
endif
ifeq ($(wildcard $(BIN_PATH)),)
 $(shell mkdir -p $(BIN_PATH))
endif

############################################################################################################################

SRC1    = $(addprefix $(SRC1_PATH), $(SRC1_FILE))         
SRC2    = $(addprefix $(SRC2_PATH), $(SRC2_FILE))         
SRC3    = $(addprefix $(SRC3_PATH), $(SRC3_FILE)) 
SRCS    = $(SRC1) $(SRC2) $(SRC3) 

OBJ1	= $(addprefix $(OBJ_PATH)o1/, $(addsuffix .o1, $(notdir $(basename $(SRC1)))))
OBJ2	= $(addprefix $(OBJ_PATH)o2/, $(addsuffix .o2, $(notdir $(basename $(SRC1))))) \
          $(addprefix $(OBJ_PATH)o2/, $(addsuffix .o2, $(notdir $(basename $(SRC2)))))
OBJ3	= $(addprefix $(OBJ_PATH)o3/, $(addsuffix .o3, $(notdir $(basename $(SRC1))))) \
		  $(addprefix $(OBJ_PATH)o3/, $(addsuffix .o3, $(notdir $(basename $(SRC3)))))
		  
OBJ4	= $(addprefix $(OBJ_PATH)o4/, $(addsuffix .o4, $(notdir $(basename $(SRC1))))) \
		  $(addprefix $(OBJ_PATH)o4/, $(addsuffix .o4, $(notdir $(basename $(SRC3)))))		  

DEPS	= $(OBJ1:.o1=.d) $(OBJ2:.o2=.d) $(OBJ3:.o3=.d)
MISS	= $(filter-out $(wildcard $(DEPS)),$(DEPS))
DEFS	= $(addprefix -D, $(PRE_DEFINE)) 
INCS	= $(addprefix -I, $(INC_FILE) $(INC_PATH)) 
LIBS	= $(addprefix -L, $(LIB_PATH)) $(addprefix -l, $(LIB_FILE)) 

############################################################################################################################

TARGET_LIBHEADZ	= $(addprefix $(BIN_PATH), libHeadz.a)
TARGET_EXAMPLES	= $(addprefix $(BIN_PATH), ntest.out)
TARGET_LINUX_UT	= $(addprefix $(BIN_PATH), utest.out)
TARGET_WIN32_UT	= $(addprefix $(BIN_PATH), utest.exe)

.PHONY: all lib ut eg clean release backup 
all: ut backup

lib: $(TARGET_LIBHEADZ)

ut: $(TARGET_LINUX_UT)
	@$(TARGET_LINUX_UT) 
	@for i in $(SRC1); do                       \
		gcov $$i -o $(OBJ_PATH)o3 > /dev/null;  \
	done
	@mv *.gcov $(BIN_PATH)
	
eg: $(TARGET_EXAMPLES)
	@$(TARGET_EXAMPLES)
	@mv gmon.out $(BIN_PATH)
	@gprof $(TARGET_EXAMPLES) $(BIN_PATH)gmon.out > $(TARGET_EXAMPLES).prof

dut: $(TARGET_LINUX_UT); gdb -w $(TARGET_LINUX_UT)
deg: $(TARGET_EXAMPLES); gdb -w $(TARGET_EXAMPLES)	

win: $(TARGET_WIN32_UT)
	@$(TARGET_WIN32_UT) -v
	
############################################################################################################################

CC       = gcc -pipe
CFLAGS   = -Wall -g -MMD -O -Wno-implicit -Wshadow -Wpointer-arith -Werror -Waggregate-return \
		   -Wparentheses -Wuninitialized	\
		   #-fstack-check -fstack-protector -pedantic	\
		   #-std=c99 

CXX      = g++ -pipe
CXXFLAGS = -Wall -g -MMD 

EGFLAGS  = -pg 

UTFLAGS  = -fprofile-arcs -ftest-coverage 	\
		   -Dmalloc=stub_malloc -Dfree=stub_free -Dcalloc=stub_calloc -Drealloc=stub_realloc

MSVC     = D:/Proj/MSVC71
		   
MCL      = $(MSVC)/Bin/CL.EXE
MCL_FLAG = /nologo /MLd /W3 /O2 /c /I $(MSVC)/Include /D _WIN32 \
		   /D malloc=stub_malloc /D free=stub_free /D calloc=stub_calloc /D realloc=stub_realloc

MLD      = $(MSVC)/Bin/LINK.EXE
MLD_FLAG = /nologo /SUBSYSTEM:CONSOLE /LIBPATH:$(MSVC)/Lib/ WS2_32.LIB KERNEL32.LIB WINMM.LIB
	   
############################################################################################################################

$(TARGET_LIBHEADZ) : $(OBJ1) ; @$(AR) $(ARFLAGS) $@  $^ > /dev/null   

$(TARGET_EXAMPLES) : $(OBJ2) ; @$(CXX) $(OBJ2) $(LIBS) $(EGFLAGS) -o $@ 

$(TARGET_LINUX_UT) : $(OBJ3) 
	@$(CXX) $(OBJ3) $(LIBS) $(UTFLAGS) -o $@ 
	@nm -l -n -r  $(TARGET_LINUX_UT) > $(basename $(TARGET_LINUX_UT)).sym
	
$(TARGET_WIN32_UT) : $(OBJ4) 
	@$(MLD) $(MLD_FLAG) /out:$@ $(OBJ4)   

%.o1 : 
	@echo $(CC) $(CFLAGS) ... $(notdir $(basename $@)).c 
	@$(CC) $(DEFS) $(INCS) -E $(filter %/$(notdir $*).c ,$(SRC1)) > $(basename $@).i 
	@$(CC) $(DEFS) $(INCS) -S $(filter %/$(notdir $*).c ,$(SRC1)) -o $(basename $@).s 
	@$(CC) $(DEFS) $(INCS) -c $(CFLAGS) -o $@ $(filter %/$(notdir $*).c ,$(SRC1))
	@objdump -ldrS $@ > $(basename $@).lst
	@python assert.py $(filter %/$(notdir $*).c ,$(SRC1))

%.o2 : 
	@echo $(CXX) ... $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC2))
	@$(CXX) $(DEFS) $(INCS) -E $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC2)) > $(basename $@).i 
	@$(CXX) $(DEFS) $(INCS) -S $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC2)) -o $(basename $@).s 
	@$(CXX) $(DEFS) $(INCS) -c $(CXXFLAGS) $(EGFLAGS) -o $@ $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC2))
	@objdump -ldrS $@ > $(basename $@).lst

%.o3 : 
	@echo $(CXX) ... $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC3))
	@$(CXX) $(DEFS) $(INCS) -E $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC3)) > $(basename $@).i 
	@$(CXX) $(DEFS) $(INCS) -S $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC3)) -o $(basename $@).s 
	@$(CXX) $(DEFS) $(INCS) -c $(CXXFLAGS) $(UTFLAGS) -o $@ $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC3))
	@objdump -ldrS $@ > $(basename $@).lst

%.o4 : 
	@echo $(MCL) ... $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC3))
	@$(MCL) $(MCL_FLAG) $(INCS) /Fo$@ /Fa$(basename $@).lst -o $@ $(filter %/$(notdir $*).c %/$(notdir $*).cpp ,$(SRC1) $(SRC3))
	
############################################################################################################################

ifneq ($(MISS),) 
$(MISS) : 
	@rm -f $(patsubst %.d, %.o1, $@)
	@rm -f $(patsubst %.d, %.o2, $@)
	@rm -f $(patsubst %.d, %.o3, $@)
endif 

-include $(DEPS)  

############################################################################################################################

REVISION = $(shell expr $(shell cat revision) + 1)

version ver :
	@echo -e "\033[1;34m"										
	@echo Current version is  Headz_$(VERSION)-$(shell cat revision).tar.gz 	
	@echo -e "\033[0m"
revision++ r++:
	@echo $(VERSION)-$(shell cat revision) --\> $(VERSION)-$(shell expr $(shell cat revision) + 1)
	@echo $(shell expr $(shell cat revision) + 1) > revision
revision-- r--:
	@echo $(VERSION)-$(shell cat revision) --\> $(VERSION)-$(shell expr $(shell cat revision) - 1)
	@echo $(shell expr $(shell cat revision) - 1) > revision

release rel : clean
	@rm -rf ../../Headz_$(VERSION)-$(REVISION)*
	@mkdir ../../Headz_$(VERSION)-$(REVISION)
	@echo $(REVISION) > revision 
	@cp -rf ../* ../../Headz_$(VERSION)-$(REVISION)/
	@cd ../../ && tar zcvf Headz_$(VERSION)-$(REVISION).tar.gz Headz_$(VERSION)-$(REVISION) > /dev/null
	@rm -rf ../../Headz_$(VERSION)-$(REVISION)
	@if test -f ../../Headz_$(VERSION)-$(REVISION).tar.gz ; 	\
	 	then													\
	 		echo -e "\033[1;34m"								\
	 	 && echo Headz_$(VERSION)-$(REVISION).tar.gz done. ;	\
	 	else													\
	 		echo $(shell expr $(REVISION) - 1) > revision		\
	 	 && echo -e "\033[1;31m"								\
	 	 && echo Headz_$(VERSION)-$(REVISION).tar.gz dont. ;	\
	 fi
	@echo -e "\033[0m"

# http://pueblo.sourceforge.net/doc/manual/ansi_color_codes.html
   
backup bak :
	@cd ../ && tar zcvf ../Headz_$(shell date +%Y.%m.%d-%H.%M.%S).tar.gz src tst egg > /dev/null

clean :  
	@rm -rf $(BIN_PATH) $(OBJ_PATH)
	@rm -rf *.ncb *.opt *.plg *.suo *.gcno *.gcda *.gcov *.log *.stackdump	
