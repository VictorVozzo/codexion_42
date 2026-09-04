/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 09:58:54 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/04 16:28:37 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void swap(t_coder **first_element, t_coder **second_element)
{
	t_coder *temp;

	temp = *first_element;
	*first_element = *second_element;
	*second_element = temp;
}

static int has_higher_priority(t_hub *hub, t_coder *coder_a, t_coder *coder_b)
{
	long long deadline_a;
	long long deadline_b;

	if (strcmp(hub->args->scheduler, "edf") == 0)
	{
		deadline_a = coder_a->last_compile_start + hub->args->time_to_burnout;
		deadline_b = coder_b->last_compile_start + hub->args->time_to_burnout;
		if (deadline_a != deadline_b)
			return (deadline_a < deadline_b);
		return (coder_a->enter_queue_time < coder_b->enter_queue_time);
	}
	return (coder_a->enter_queue_time < coder_b->enter_queue_time);
}

void queue_push(t_dongle *dongle, t_coder *coder, t_hub *hub)
{
	int idx;
	int parent;

	coder->enter_queue_time = get_time_in_ms();
	idx = dongle->coder_queue_size;
	dongle->coder_queue[idx] = coder;
	dongle->coder_queue_size++;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (has_higher_priority(hub, dongle->coder_queue[idx],
								dongle->coder_queue[parent]))
		{
			swap(&dongle->coder_queue[idx], &dongle->coder_queue[parent]);
			idx = parent;
		}
		else
			break;
	}
}

static void heapify_down(t_dongle *dongle, int idx, t_hub *hub)
{
	int smallest;
	int left;
	int right;

	while (1)
	{
		smallest = idx;
		left = 2 * idx + 1;
		right = 2 * idx + 2;
		if (left < dongle->coder_queue_size &&
			has_higher_priority(hub, dongle->coder_queue[left],
								dongle->coder_queue[smallest]))
			smallest = left;
		if (right < dongle->coder_queue_size &&
			has_higher_priority(hub, dongle->coder_queue[right],
								dongle->coder_queue[smallest]))
			smallest = right;
		if (smallest == idx)
			break;
		swap(&dongle->coder_queue[idx], &dongle->coder_queue[smallest]);
		idx = smallest;
	}
}

void queue_pop(t_dongle *dongle, t_hub *hub)
{
	if (dongle->coder_queue_size == 0)
		return;
	dongle->coder_queue[0] =
		dongle->coder_queue[dongle->coder_queue_size - 1];
	dongle->coder_queue_size--;
	dongle->coder_queue[dongle->coder_queue_size] = NULL;
	if (dongle->coder_queue_size > 0)
		heapify_down(dongle, 0, hub);
}
