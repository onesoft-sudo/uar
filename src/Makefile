CC = musl-gcc
CFLAGS = -std=c99 -g -O2 -I. -Wall -Wextra -Werror -pedantic -DHAVE_CONFIG_H -DNDEBUG
LDLIBS = -lm
RM = rm -f
AS = as
ASFLAGS = -g

all: uar bin-exec.uar

uar: libuar.a main.o
	@echo "  CCLD  $@"
	@$(CC) $(CFLAGS) -o $@ main.o -L. -luar $(LDLIBS)

%.o: %.s
	@echo "  AS    $@"
	@$(AS) $(ASFLAGS) -c -o $@ $<

%.o: %.c
	@echo "  CC    $@"
	$(CC) $(CFLAGS) -c -o $@ $<

libuar.a: uar.o xmalloc.o io.o
	@echo "  AR    $@"
	@ar rcs $@ $^

bin-exec.uar: libuar.a selfext.o selfext_entry.o
	@echo "  CCLD  $@"
	@$(CC) -static $(LDFLAGS) -o $@ selfext.o selfext_entry.o -L. -luar $(LDLIBS)

clean:
	@echo "  RM bin-exec"
	@echo "  RM uar"
	@echo "  RM *.o"
	@$(RM) uar *.o bin-exec *.a