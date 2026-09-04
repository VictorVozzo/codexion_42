# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 0026/08/23 19:36:02 by ivozzo-           #+#    #+#              #
#    Updated: 2026/09/04 13:18:06 by vivozzo-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRCS = args_creation.c \
       coders_creation.c \
       coders_routine.c \
       dongles_creation.c \
       dongles_first.c \
       dongles_second.c \
       hub_creation.c \
       main.c \
       monitor.c \
       queue_utils.c \
       time_utils.c \
       utils_parsing.c \
       utils.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all