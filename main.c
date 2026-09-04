/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 10:30:04 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/03 16:42:33 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	all_args_verif(int total_args, char **args)
{
	if (!args_size_verif(total_args) || !args_verif(args))
		return (0);
	return (1);
}

int	main(int argc, char **argv)
{
	t_args	*arguments;
	t_hub	*hub;

	if (!(all_args_verif(argc, argv)))
		return (1);
	arguments = create_t_args(argv);
	if (!arguments)
		return(1);
	hub = create_hub(arguments);
	if (!hub)
		return(1);
	init_simulation(hub);
	free_all(hub);
}
