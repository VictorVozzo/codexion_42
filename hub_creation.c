/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub_creation.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 11:24:32 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/04 11:32:18 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void free_error(t_hub *hub, int flag)
{
    if (flag == 1)
    {
        clean_mutex_coders(hub->args->number_of_coders - 1, hub->coders);
        clean_mutex_cond_dongles(hub->args->number_of_coders - 1,
                                 hub->dongles);
        free(hub->args);
        free(hub);
    }
    else if (flag == 2)
    {
        clean_mutex_coders(hub->args->number_of_coders - 1, hub->coders);
        clean_mutex_cond_dongles(hub->args->number_of_coders - 1,
                                 hub->dongles);
        pthread_mutex_destroy(&hub->print_log_mutex);
        free(hub->args);
        free(hub);
    }
}

t_hub *create_hub(t_args *args)
{
    t_hub *hub;

    hub = malloc(sizeof(t_hub));
    if (!hub)
        return (NULL);
    hub->args = args;
    hub->dongles = create_dongles(args);
    if (!hub->dongles)
        return (free(hub->args), free(hub), NULL);
    hub->coders = create_coders(hub);
    if (!hub->coders)
        return (free(hub->dongles), free(hub->args), free(hub), NULL);
    hub->start_time = 0;
    hub->simulation_running = false;
    if (pthread_mutex_init(&hub->print_log_mutex, NULL) != 0)
        return (free_error(hub, 1), NULL);
    if (pthread_mutex_init(&hub->monitor_mutex, NULL) != 0)
        return (free_error(hub, 2), NULL);
    hub->monitor_finished = false;
    return (hub);
}
