NAME = philo

SRCS = main.c

FLAGS = -Wall -Wextra -Werror -pthread

HEADER = philo.h

SANITIZER = -fsanitize=thread -flto -Ofast #-fsanitize=address

all: $(NAME)

$(NAME): $(SRCS) $(HEADER)
	@cc $(FLAGS) -o $(NAME) $(SRCS) $(SANITIZER)

clean:
	@rm -f $(NAME)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re