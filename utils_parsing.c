/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parsing.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 11:08:21 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/07 10:09:20 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_integer_gt_zero(char *str)
{
	int			i;
	long long	n;

	i = 0;
	n = 0;
	if (str[0] == '-' || str[i] == '\0')
		return (0);
	if (str[0] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		n = n * 10 + (str[i] - '0');
		if (n > __INT_MAX__)
			return (0);
		i++;
	}
	return (1);
}

int	args_size_verif(int number_args)
{
	if (number_args != 9)
	{
		fprintf(stderr,
			"Expected 8 arguments, got %d.\n"
			"\n1º required argument: number_of_coders "
			"(must be an integer greater or equal than 0)."
			"\n2º required argument: time_to_burnout (ms) "
			"(must be an integer greater or equal than 0)."
			"\n3º required argument: time_to_compile (ms) "
			"(must be an integer greater or equal than 0)."
			"\n4º required argument: time_to_debug (ms) "
			"(must be an integer greater or equal than 0)."
			"\n5º required argument: time_to_refactor (ms) "
			"(must be an integer greater or equal than 0)."
			"\n6º required argument: number_of_compiles_required "
			"(must be an integer greater or equal than 0)."
			"\n7º required argument: dongle_cooldown (ms) "
			"(must be an integer greater or equal than 0)."
			"\n8º required argument: scheduler "
			"(must be 'fifo' or 'edf')\n",
			number_args - 1);
		return (0);
	}
	return (1);
}

static void	copy_error_msg(char *error_msg, char *new_msg)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (error_msg[i])
		i++;
	while (new_msg[j])
	{
		error_msg[i + j] = new_msg[j];
		j++;
	}
	error_msg[i + j] = '\0';
}

static void	args_verif_auxil(char **args, char *error_msg)
{
	if (!is_integer_gt_zero(args[1]))
		copy_error_msg(error_msg,
			"number_of_coders "
			"must be an integer >= 0\n");
	if (!is_integer_gt_zero(args[2]))
		copy_error_msg(error_msg,
			"time_to_burnout (ms) must be an integer >= 0\n");
	if (!is_integer_gt_zero(args[3]))
		copy_error_msg(error_msg,
			"time_to_compile (ms) must be an integer >= 0\n");
	if (!is_integer_gt_zero(args[4]))
		copy_error_msg(error_msg,
			"time_to_debug (ms) must be an integer >= 0\n");
	if (!is_integer_gt_zero(args[5]))
		copy_error_msg(error_msg,
			"time_to_refactor (ms) must be an integer >= 0\n");
	if (!is_integer_gt_zero(args[6]))
		copy_error_msg(error_msg,
			"number_of_compiles_required "
			"must be an integer >= 0\n");
	if (!is_integer_gt_zero(args[7]))
		copy_error_msg(error_msg,
			"dongle_cooldown (ms) must be an integer >= 0\n");
	if (strcmp(args[8], "fifo") != 0 && strcmp(args[8], "edf") != 0)
		copy_error_msg(error_msg, "scheduler must be 'fifo' or 'edf'\n");
}

int	args_verif(char **args)
{
	char	error_msg[352];

	memset(error_msg, '\0', sizeof(error_msg));
	args_verif_auxil(args, error_msg);
	if (strlen(error_msg) == 0)
		return (1);
	fprintf(stderr, "%s", error_msg);
	return (0);
}
