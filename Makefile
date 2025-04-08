# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/08 14:46:08 by lucmansa          #+#    #+#              #
#    Updated: 2025/04/08 15:04:45 by lucmansa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = philo
CC = cc
FLAGS = -Wall -Wextra -Werror -g

SRCS_DIR = srcs/
INC_DIR = includes/

SRCS = manage_guard.c manage_philo.c philo.c utils_all.c
OBJECTS = $(addprefix $(SRCS_DIR), $(SRCS:.c=.o))

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(NAME)

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR)

clean:
		rm -rf $(OBJECTS)


fclean: clean
		rm -rf $(NAME)

re: fclean all