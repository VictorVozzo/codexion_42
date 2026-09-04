/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_routine.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 14:17:57 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/04 11:24:37 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void *start_simulation(void *arg)
{
	t_coder *coder;

	coder = (t_coder *)arg;

	while (coder->hub->args->number_of_compiles_required != coder->total_compiles)
	{
		pthread_mutex_lock(&coder->hub->monitor_mutex);
		if (coder->hub->monitor_finished == true)
		{
			pthread_mutex_unlock(&coder->hub->monitor_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&coder->hub->monitor_mutex);
		get_first_dongle(coder);
	}
	return (NULL);
}

static int join_coders_threads(t_hub *hub)
{
	int i;

	i = 0;
	while (i < hub->args->number_of_coders)
	{
		if (pthread_join(hub->coders[i].thread_id, NULL) != 0)
			return (-1);
		i++;
	}
	return (0);
}

static int start_coders_threads(t_hub *hub)
{
	int i;

	i = 0;
	while (i < hub->args->number_of_coders)
	{
		if (pthread_create(&hub->coders[i].thread_id, NULL, start_simulation,
						   &hub->coders[i]) != 0)
			return (-1);
		i++;
	}
	return (0);
}

void init_simulation(t_hub *hub)
{
	hub->start_time = get_time_in_ms();
	hub->simulation_running = true;
	start_monitor_thread(hub);
	start_coders_threads(hub);
	join_coders_threads(hub);
	join_monitor_thread(hub);
}
