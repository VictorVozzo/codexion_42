/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_creation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 10:30:04 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/04 11:17:54 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void clean_mutex_coders(int index, t_coder *coders_array)
{
	while (index != -1)
	{
		pthread_mutex_destroy(&coders_array[index].coder_mutex);
		index--;
	}
	free(coders_array);
}

t_coder *create_coders(t_hub *hub)
{
	int i;
	t_coder *coders_array;

	coders_array = malloc(hub->args->number_of_coders * sizeof(t_coder));
	if (!coders_array)
		return (NULL);
	i = 0;
	while (i != hub->args->number_of_coders)
	{
		if (pthread_mutex_init(&coders_array[i].coder_mutex, NULL) != 0)
			return (clean_mutex_coders(i - 1, coders_array), NULL);
		coders_array[i].id_coder = i + 1;
		coders_array[i].left_dongle = &hub->dongles[i];
		if (i == hub->args->number_of_coders - 1)
			coders_array[i].right_dongle = &hub->dongles[0];
		else
			coders_array[i].right_dongle = &hub->dongles[i + 1];
		coders_array[i].last_compile_start = 0;
		coders_array[i].enter_queue_time = 0;
		coders_array[i].total_compiles = 0;
		coders_array[i].hub = hub;
		i++;
	}
	return (coders_array);
}
