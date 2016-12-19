##############################################################################
# Path

SRCROOT := $(PWD)
DESTDIR := $(SRCROOT)/out
LIB_PATH   := /usr/lib
INC_PATH   := /usr/include

CFLAGS	:= $(EXTRA_CFLAGS)
LDFLAGS	:= $(EXTRA_LDFLAGS)


###############################################################################
# Compile

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib

TARGET	= libat2.a
CFLAGS += -W -Wall
HEADERS = include/at


LIB_SRCS  = src/at_cmd_table_wrapper.c src/at_log.c src/at_misc.c src/at_thread.c src/at_thread_poll_sms.c src/at_util.c
LIB_SRCS += src/at_cmd_dev_tx500.c
LIB_SRCS += src/at_parser/at_cmd_parser_3gpp.c src/at_parser/at_cmd_parser_3gpp_utils.c
LIB_SRCS += src/watchdog.c src/client_socket_ipc.c src/server_socket_ipc.c
LIB_OBJS = $(LIB_SRCS:.c=.o)

LIB_LIBS += -lpthread -llogd
LIB_INCS = -Iinclude -Isrc -Isrc/at_parser

#####################################
TEST_SRCS = testapp/at_test.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

TEST_LIBS += -lat2 -lpthread -llogd


#all: all-before libs $(BIN) samples all-after
all: $(TARGET) install at_sample

.c.o:
	$(CC) $(CFLAGS) $(LIB_LIBS) $(LIB_INCS) -c $< -o $@

install:
	fakeroot cp -va $(TARGET) $(DESTDIR)$(LIB_PATH)
	fakeroot cp -va $(HEADERS) $(DESTDIR)$(INC_PATH)

at_sample : $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(TEST_LIBS) 
	
$(TARGET): $(LIB_OBJS)
	$(AR) rsc $@ $^
	$(RANLIB) $@

clean:
	rm -vrf $(TARGET) $(LIB_OBJS) $(LIB_OBJS)
	rm -vrf $(DESTDIR)$(LIB_PATH)/$(TARGET)
	rm -vrf $(DESTDIR)$(INC_PATH)/at
	
	