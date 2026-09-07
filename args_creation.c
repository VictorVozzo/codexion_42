/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args_creation.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 11:23:29 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/07 09:45:49 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_args	*create_t_args(char **args)
{
	t_args	*arguments;

	arguments = malloc(sizeof(t_args));
	if (!arguments)
		return (NULL);
	arguments->number_of_coders = atoi(args[1]);
	arguments->time_to_burnout = atoi(args[2]);
	arguments->time_to_compile = atoi(args[3]);
	arguments->time_to_debug = atoi(args[4]);
	arguments->time_to_refactor = atoi(args[5]);
	arguments->number_of_compiles_required = atoi(args[6]);
	arguments->dongle_cooldown = atoi(args[7]);
	arguments->scheduler = args[8];
	return (arguments);
}
