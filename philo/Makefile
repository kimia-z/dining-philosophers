NAME = philo

SRCS = main.c initials.c actions.c threads.c forks.c dead_check.c utils.c

FLAGS = -Wall -Wextra -Werror -pthread -lrt

HEADER = philo.h

SANITIZER = -fsanitize=thread  #-fsanitize=address

all: $(NAME)

$(NAME): $(SRCS) $(HEADER)
	@cc $(FLAGS) -o $(NAME) $(SRCS) $(SANITIZER)

clean:
	@rm -f $(NAME)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re