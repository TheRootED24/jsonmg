NAME ?= jsonmg 
PROG ?= jsonmg.so
DELETE = rm -rf
OUT ?= -o $(PROG)
LOUT ?= -o $(LPROG)
SOURCES = jsonmg.c
CFLAGS = -W -Wall -Wextra -O9
LDFLAGS = -shared -fpic -llua -lm

all: $(NAME)

$(NAME): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) $(LDFLAGS) $(OUT)

clean:
	$(DELETE) *.o *.so
