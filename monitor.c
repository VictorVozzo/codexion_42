/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 10:18:03 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/07 10:22:08 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	monitor_check_burnout(t_hub *hub, int index)
{
	long long	deadline;

	pthread_mutex_lock(&hub->coders[index].coder_mutex);
	if (hub->coders[index].last_compile_start == 0)
		deadline = hub->start_time + hub->args->time_to_burnout;
	else
		deadline = hub->coders[index].last_compile_start
			+ hub->args->time_to_burnout;
	if (get_time_in_ms() >= deadline)
	{
		pthread_mutex_unlock(&hub->coders[index].coder_mutex);
		print_output_msg(&hub->coders[index], "burned out");
		pthread_mutex_lock(&hub->monitor_mutex);
		hub->monitor_finished = true;
		hub->simulation_running = false;
		pthread_mutex_unlock(&hub->monitor_mutex);
		wake_up_all_dongles(hub);
		return (1);
	}
	pthread_mutex_unlock(&hub->coders[index].coder_mutex);
	return (0);
}

static int	monitor_check_compilations(t_hub *hub)
{
	int	all_done;
	int	i;

	all_done = 1;
	i = 0;
	while (i < hub->args->number_of_coders)
	{
		pthread_mutex_lock(&hub->coders[i].coder_mutex);
		if (hub->coders[i].total_compiles
			< hub->args->number_of_compiles_required)
			all_done = 0;
		pthread_mutex_unlock(&hub->coders[i].coder_mutex);
		if (!all_done)
			break ;
		i++;
	}
	if (all_done)
	{
		pthread_mutex_lock(&hub->monitor_mutex);
		hub->monitor_finished = true;
		pthread_mutex_unlock(&hub->monitor_mutex);
		wake_up_all_dongles(hub);
		return (1);
	}
	return (0);
}

static void	*monitor_thread(void *arg)
{
	t_hub	*hub;
	int		i;

	hub = (t_hub *)arg;
	i = 0;
	while (!monitor_check_compilations(hub))
	{
		if (monitor_check_burnout(hub, i))
			return (NULL);
		i++;
		if (i == hub->args->number_of_coders)
			i = 0;
		usleep(1000);
	}
	return (NULL);
}

int	join_monitor_thread(t_hub *hub)
{
	if (pthread_join(hub->monitor_thread_id, NULL) != 0)
		return (-1);
	return (0);
}

int	start_monitor_thread(t_hub *hub)
{
	if (pthread_create(&hub->monitor_thread_id, NULL, monitor_thread, hub) != 0)
		return (-1);
	return (0);
}
