/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_creation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 10:30:04 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/07 09:24:35 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	clean_mutex_cond_dongles(int index, t_dongle *dongles_array)
{
	while (index != -1)
	{
		pthread_mutex_destroy(&dongles_array[index].mutex);
		pthread_cond_destroy(&dongles_array[index].cond);
		index--;
	}
	free(dongles_array);
}

t_dongle	*create_dongles(t_args *args)
{
	int			i;
	t_dongle	*dongles_array;

	dongles_array = malloc(args->number_of_coders * sizeof(t_dongle));
	if (!dongles_array)
		return (NULL);
	i = 0;
	while (i != args->number_of_coders)
	{
		if (pthread_mutex_init(&dongles_array[i].mutex, NULL) != 0)
			return (clean_mutex_cond_dongles(i - 1, dongles_array), NULL);
		if (pthread_cond_init(&dongles_array[i].cond, NULL) != 0)
			return (pthread_mutex_destroy(&dongles_array[i].mutex),
				clean_mutex_cond_dongles(i - 1, dongles_array), NULL);
		dongles_array[i].available = true;
		dongles_array[i].time_available = 0;
		dongles_array[i].coder_queue[0] = NULL;
		dongles_array[i].coder_queue[1] = NULL;
		dongles_array[i].coder_queue_size = 0;
		i++;
	}
	return (dongles_array);
}
