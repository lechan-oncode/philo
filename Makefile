# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/17 21:51:33 by lechan            #+#    #+#              #
#    Updated: 2025/03/18 03:41:43 by lechan           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = philo
SRC_DIR     = src
SRCS        = $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR     = obj
OBJS        = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -pthread #-fsanitize=thread -g3 -ggdb
RM          = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
