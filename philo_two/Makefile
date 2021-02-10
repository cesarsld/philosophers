# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/11/18 18:36:00 by cjaimes           #+#    #+#              #
#    Updated: 2021/02/10 13:44:52 by cjaimes          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

OBJ_DIR		=	obj/

SRC_DIR		=	srcs/

INCLUDE		=	includes

SRC			=	main.c \
				monitor.c \
				utils.c \
				time.c \
				philo_life.c
				

SRCS		=	${addprefix ${SRC_DIR}, ${SRC}}

OBJS		=	${patsubst ${SRC_DIR}%.c,${OBJ_DIR}%.o,${SRCS}}

NAME		=	philo_two

RM			=	rm -f

CC			=	clang

CFLAGS		=	-Wall -Wextra -Werror -g -O3

T		=	$(words ${OBJS})
N		=	0
C		=	$(words $N)${eval N := X $N}
_CYAN	=	\033[36m
_GREEN	=	\033[32m
ECHO	=	"[`expr $C  '*' 100 / $T`%]"

all:	${NAME}

${OBJ_DIR}%.o :	${SRC_DIR}%.c
			@mkdir -p ${OBJ_DIR}
			@${CC} ${CFLAGS} -I ${INCLUDE} -c $< -o $@
			@printf "%-60b\r" "${_CYAN}${ECHO}${_CYAN} Compiling $@"

${NAME}: ${OBJS}
	${CC} ${CFLAGS} -lpthread -o ${NAME} ${OBJS}

run : all
	./${NAME} 4 410 200 200 5

val : all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./${NAME} 4 610 200 200 5

bonus: all

clean:
	${RM} ${OBJS}

fclean:	clean
	${RM} ${NAME}

re: fclean all