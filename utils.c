/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 15:38:47 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/04 11:34:07 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void print_output_msg(t_coder *coder, const char *msg)
{
	long long timestamp;

	pthread_mutex_lock(&coder->hub->monitor_mutex);
	pthread_mutex_lock(&coder->hub->print_log_mutex);
	if (coder->hub->simulation_running == true)
	{
		timestamp = get_time_in_ms() - coder->hub->start_time;
		printf("%lld %d %s\n", timestamp, coder->id_coder, msg);
	}
	pthread_mutex_unlock(&coder->hub->print_log_mutex);
	pthread_mutex_unlock(&coder->hub->monitor_mutex);
}

void free_all(t_hub *hub)
{
	clean_mutex_coders(hub->args->number_of_coders - 1, hub->coders);
	clean_mutex_cond_dongles(hub->args->number_of_coders - 1, hub->dongles);
	pthread_mutex_destroy(&hub->print_log_mutex);
	pthread_mutex_destroy(&hub->monitor_mutex);
	free(hub->args);
	free(hub);
}

void wake_up_all_dongles(t_hub *hub)
{
	int i;

	i = 0;
	while (i < hub->args->number_of_coders)
	{
		pthread_cond_broadcast(&hub->dongles[i].cond);
		i++;
	}
}
