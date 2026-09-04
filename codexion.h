/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vivozzo- <vivozzo-@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 10:30:39 by vivozzo-          #+#    #+#             */
/*   Updated: 2026/09/04 14:26:29 by vivozzo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
#define CODEXION_H

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct s_hub t_hub;
typedef struct s_dongle t_dongle;
typedef struct s_coder t_coder;

typedef struct s_args
{
	int number_of_coders;
	int time_to_burnout;
	int time_to_compile;
	int time_to_debug;
	int time_to_refactor;
	int number_of_compiles_required;
	int dongle_cooldown;
	char *scheduler;
} t_args;

typedef struct s_coder
{
	pthread_t thread_id;
	pthread_mutex_t coder_mutex;
	int id_coder;
	t_dongle *left_dongle;
	t_dongle *right_dongle;
	long long last_compile_start;
	long long enter_queue_time;
	int total_compiles;
	t_hub *hub;
} t_coder;

typedef struct s_dongle
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	bool available;
	long long time_available;
	t_coder *coder_queue[2];
	int coder_queue_size;
} t_dongle;

typedef struct s_hub
{
	t_args *args;
	t_dongle *dongles;
	t_coder *coders;
	long long start_time;
	bool simulation_running;
	pthread_mutex_t print_log_mutex;
	pthread_t monitor_thread_id;
	pthread_mutex_t monitor_mutex;
	bool monitor_finished;
} t_hub;

t_args *create_t_args(char **args);

void clean_mutex_coders(int index, t_coder *coders_array);
t_coder *create_coders(t_hub *hub);

void init_simulation(t_hub *hub);

void clean_mutex_cond_dongles(int index, t_dongle *dongles_array);
t_dongle *create_dongles(t_args *args);

void check_dongle_availability(t_coder *coder, t_dongle *dongle);
void get_first_dongle(t_coder *coder);

int get_second_dongle(t_coder *coder);

t_hub *create_hub(t_args *args);

int join_monitor_thread(t_hub *hub);
int start_monitor_thread(t_hub *hub);

void queue_push(t_dongle *dongle, t_coder *coder, t_hub *hub);
void queue_pop(t_dongle *dongle, t_hub *hub);

long long get_time_in_ms(void);

int args_size_verif(int number_args);
int args_verif(char **args);

void print_output_msg(t_coder *coder, const char *msg);
void free_all(t_hub *hub);
void wake_up_all_dongles(t_hub *hub);

#endif
