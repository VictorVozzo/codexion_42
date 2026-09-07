/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_second.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 13:17:46 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/07 10:15:10 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	debug_refactor(t_coder *coder)
{
	pthread_mutex_lock(&coder->right_dongle->mutex);
	coder->right_dongle->available = true;
	coder->right_dongle->time_available = get_time_in_ms();
	pthread_cond_broadcast(&coder->right_dongle->cond);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
	if (coder->left_dongle != coder->right_dongle)
	{
		pthread_mutex_lock(&coder->left_dongle->mutex);
		coder->left_dongle->available = true;
		coder->left_dongle->time_available = get_time_in_ms();
		pthread_cond_broadcast(&coder->left_dongle->cond);
		pthread_mutex_unlock(&coder->left_dongle->mutex);
	}
	print_output_msg(coder, "is debugging");
	usleep(coder->hub->args->time_to_debug * 1000);
	print_output_msg(coder, "is refactoring");
	usleep(coder->hub->args->time_to_refactor * 1000);
}

static t_dongle	*verify_second_dongle(t_coder *coder)
{
	t_dongle	*second_dongle;

	pthread_mutex_lock(&coder->hub->monitor_mutex);
	if (coder->hub->monitor_finished == true)
		return (pthread_mutex_unlock(&coder->hub->monitor_mutex), NULL);
	pthread_mutex_unlock(&coder->hub->monitor_mutex);
	if (coder->id_coder % 2 == 0)
		second_dongle = coder->right_dongle;
	else
		second_dongle = coder->left_dongle;
	pthread_mutex_lock(&second_dongle->mutex);
	if (second_dongle->coder_queue[0] != coder
		&& second_dongle->coder_queue[1] != coder)
		queue_push(second_dongle, coder, coder->hub);
	return (second_dongle);
}

static int	get_second_dongle_auxil(t_coder *coder, t_dongle *second_dongle)
{
	second_dongle->available = false;
	queue_pop(second_dongle, coder->hub);
	pthread_mutex_unlock(&second_dongle->mutex);
	print_output_msg(coder, "has taken a dongle");
	pthread_mutex_lock(&coder->coder_mutex);
	coder->last_compile_start = get_time_in_ms();
	coder->total_compiles += 1;
	pthread_mutex_unlock(&coder->coder_mutex);
	print_output_msg(coder, "is compiling");
	usleep(coder->hub->args->time_to_compile * 1000);
	debug_refactor(coder);
	return (1);
}

int	get_second_dongle(t_coder *coder)
{
	t_dongle	*second_dongle;

	second_dongle = verify_second_dongle(coder);
	if (!second_dongle)
		return (0);
	while (!(second_dongle->available == true
			&& second_dongle->coder_queue[0] == coder
			&& (get_time_in_ms() - second_dongle->time_available)
			>= coder->hub->args->dongle_cooldown))
	{
		pthread_mutex_lock(&coder->hub->monitor_mutex);
		if (coder->hub->monitor_finished == true)
			return (pthread_mutex_unlock(&coder->hub->monitor_mutex),
				pthread_mutex_unlock(&second_dongle->mutex), 0);
		pthread_mutex_unlock(&coder->hub->monitor_mutex);
		check_dongle_availability(coder, second_dongle);
	}
	return (get_second_dongle_auxil(coder, second_dongle));
}
