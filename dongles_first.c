/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_first.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 15:52:06 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/07 10:13:28 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	check_dongle_availability(t_coder *coder, t_dongle *dongle)
{
	long long		wake_at;
	struct timespec	ts;

	if (dongle->available == true)
	{
		wake_at = dongle->time_available + coder->hub->args->dongle_cooldown;
		if (wake_at <= get_time_in_ms())
			wake_at = get_time_in_ms() + 5;
		ts.tv_sec = wake_at / 1000;
		ts.tv_nsec = (wake_at % 1000) * 1000000;
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	else
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
}

static void	try_get_second_dongle(t_coder *coder, t_dongle *first_dongle)
{
	first_dongle->available = false;
	queue_pop(first_dongle, coder->hub);
	pthread_mutex_unlock(&first_dongle->mutex);
	print_output_msg(coder, "has taken a dongle");
	if (get_second_dongle(coder) == 0
		&& coder->left_dongle != coder->right_dongle)
	{
		pthread_mutex_lock(&first_dongle->mutex);
		first_dongle->available = true;
		first_dongle->time_available = get_time_in_ms();
		pthread_cond_broadcast(&first_dongle->cond);
		pthread_mutex_unlock(&first_dongle->mutex);
	}
}

static t_dongle	*verify_first_dongle(t_coder *coder)
{
	t_dongle	*first_dongle;

	pthread_mutex_lock(&coder->hub->monitor_mutex);
	if (coder->hub->monitor_finished == true)
		return (pthread_mutex_unlock(&coder->hub->monitor_mutex), NULL);
	pthread_mutex_unlock(&coder->hub->monitor_mutex);
	if (coder->id_coder % 2 == 0)
		first_dongle = coder->left_dongle;
	else
		first_dongle = coder->right_dongle;
	pthread_mutex_lock(&first_dongle->mutex);
	if (first_dongle->coder_queue[0] != coder
		&& first_dongle->coder_queue[1] != coder)
		queue_push(first_dongle, coder, coder->hub);
	return (first_dongle);
}

void	get_first_dongle(t_coder *coder)
{
	t_dongle	*first_dongle;

	first_dongle = verify_first_dongle(coder);
	if (!first_dongle)
		return ;
	while (!(first_dongle->available == true
			&& first_dongle->coder_queue[0] == coder
			&& (get_time_in_ms() - first_dongle->time_available)
			>= coder->hub->args->dongle_cooldown))
	{
		pthread_mutex_lock(&coder->hub->monitor_mutex);
		if (coder->hub->monitor_finished == true)
			return (pthread_mutex_unlock(&coder->hub->monitor_mutex),
				pthread_mutex_unlock(&first_dongle->mutex), (void)0);
		pthread_mutex_unlock(&coder->hub->monitor_mutex);
		check_dongle_availability(coder, first_dongle);
	}
	try_get_second_dongle(coder, first_dongle);
}
