CC = gcc

# CFLAGS = -Wall -Wextra -Werror -g3 -fsanitize=address
CFLAGS = -Wall -Wextra -Werror

TARGET = my_tar

SOURCES = src/main.c \
          src/my_tar_archive.c \
          src/helper_file.c \
          lib/string/my_string.c \
          lib/string/string_listnode.c

OBJECTS = $(SOURCES:.c=.o)  

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ 

.PHONY: clean fclean

clean:
	@rm $(OBJECTS)

fclean: clean
	@rm $(TARGET) 