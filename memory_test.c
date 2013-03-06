/*
Copyright (C) 2012  
Fabien Gaud <fgaud@sfu.ca>, Baptiste Lepers <baptiste.lepers@inria.fr>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <numa.h>
#include <numaif.h>

#include <sched.h>
#include <linux/unistd.h>

#include <ctype.h>

#include "machine.h"
#include "memory_test.h"

memory_bench_plugin_t plugins[] = {
      { "throughput", NULL, bench_throughtput },
      { "sequential_read", bench_seq_init, bench_seq_read },
      { "random_read", bench_rand_read_init, bench_rand_read },
};

int choosed_plugin = -1;

#define DEFAULT_BENCH_TIME (10LL) // In s
#define DEFAULT_MEMORY_BENCH_SIZE_TO_BENCH      (64*1024*1024) // In bytes

unsigned int nthreads;
unsigned int ndies;
uint64_t bench_time = DEFAULT_BENCH_TIME;

int die_on_which_to_alloc = -1;        /* Where to alloc memory? -1 =local */

uint64_t* nb_bytes_processed;     /* Number of bytes actually processed by a thread */

unsigned int nb_dies = 1;

pthread_mutex_t mutex;
pthread_cond_t go_to_work;
volatile unsigned int rdv_value = 0;
volatile unsigned int spin_rdv_value = 0;

struct timeval global_start_time;
uint64_t memory_size = 0;


unsigned long time_diff(struct timeval* start, struct timeval* stop){
   unsigned long sec_res = stop->tv_sec - start->tv_sec;
   unsigned long usec_res = stop->tv_usec - start->tv_usec;

   return 1000000*sec_res + usec_res;
}

uint64_t get_cpu_freq(void) {                                                                                                                                                  
   FILE *fd;
   uint64_t freq = 0;
   float freqf = 0;
   char *line = NULL;
   size_t len = 0;

   fd = fopen("/proc/cpuinfo", "r");
   if (!fd) {
      fprintf(stderr, "failed to get cpu frequency\n");
      perror(NULL);
      return freq;
   }   

   while (getline(&line, &len, fd) != EOF) {
      if (sscanf(line, "cpu MHz\t: %f", &freqf) == 1) {
         freqf = freqf * 1000000UL;
         freq = (uint64_t) freqf;
         break;
      }   
   }   

   fclose(fd);
   return freq;
}

static void rdv(unsigned long thread_no){
   pthread_mutex_lock(&mutex);
   rdv_value++;
   if(rdv_value < nthreads){
      pthread_cond_wait(&go_to_work, &mutex);
   }
   else{
      rdv_value = 0;
      pthread_cond_broadcast(&go_to_work);
   }
   pthread_mutex_unlock(&mutex);
}

static void spin_rdv(unsigned long thread_no) {
   // Spin-rendez-vous because if a core sleeps, PAPI discards the new counters values
   pthread_mutex_lock(&mutex);
   spin_rdv_value++;
   pthread_mutex_unlock(&mutex);
   while(spin_rdv_value < nthreads); //Spin
   pthread_mutex_lock(&mutex);
   spin_rdv_value++;
   if(spin_rdv_value == 2*nthreads)
      spin_rdv_value = 0;
   pthread_mutex_unlock(&mutex);
}

struct thread_data {
   unsigned long thread_no;
   unsigned long assigned_core;
   int do_work;
};

static pid_t gettid(void) {
   return syscall(__NR_gettid);
}

void set_affinity(int tid, int core_id) {
   cpu_set_t mask;
   CPU_ZERO(&mask);
   CPU_SET(core_id, &mask);

   int r = sched_setaffinity(tid, sizeof(mask), &mask);
   if (r < 0) {
      fprintf(stderr, "couldn't set affinity for %d\n", core_id);
      exit(1);
   }
}

static void* thread_loop(void* pdata){
   struct thread_data *tn = pdata;
   struct timeval global_stop_time, stop_time, start_time;

   /** Set thread affinity **/
   int tid = gettid();
   printf("Assigning thread %lu (tid = %d) to core %lu\n", tn->thread_no, tid, tn->assigned_core);
   set_affinity(tid, tn->assigned_core);

   uint64_t* memory_to_access = (uint64_t*) malloc(memory_size);
   assert(memory_to_access);

   if(plugins[choosed_plugin].init_fun) {
      plugins[choosed_plugin].init_fun(memory_to_access, memory_size);
   }

   rdv(tn->thread_no);

   if(tn->thread_no == 0){
      gettimeofday(&global_start_time, NULL);
   }
   gettimeofday(&start_time, NULL);


   if(tn->do_work) {
      uint64_t bytes = plugins[choosed_plugin].bench_fun(memory_to_access, memory_size, bench_time, tn->thread_no);
      nb_bytes_processed[tn->thread_no] = bytes;
      //For some reason the following line segfaults.
      //nb_bytes_processed[tn->thread_no] = plugins[choosed_plugin].bench_fun(memory_to_access, memory_size, bench_time, tn->thread_no);

      gettimeofday(&stop_time, NULL);
      spin_rdv(tn->thread_no);
   } else {
      spin_rdv(tn->thread_no);
      gettimeofday(&stop_time, NULL);
   }
   rdv(tn->thread_no);

   unsigned long length = time_diff(&start_time, &stop_time);

   if(tn->thread_no == 0){
      gettimeofday(&global_stop_time, NULL);

      unsigned long global_length = time_diff(&global_start_time, &global_stop_time);
      uint64_t total_read = 0;
      int i;
      for(i = 0; i < nthreads; i++) {
         total_read += nb_bytes_processed[i];
      }
      printf("\n");
      printf("[GLOBAL] total bytes processed: %llu\n", (long long unsigned) total_read);
      printf("[GLOBAL] test length: %.2f s (%lu us)\n", (double) global_length/1000000., global_length);
      printf("[GLOBAL] throughput: %.2f MB/s\n", ((double) total_read/1024./1024.)/ ((double) global_length/1000000.));
   }

   while(rdv_value != tn->thread_no);

   if(tn->do_work) {
      printf("\t[CORE%lu] throughput: %.2f MB/s during %.2fs\n", tn->assigned_core, ((double) nb_bytes_processed[tn->thread_no]/1024./1024.)/ ((double) length/1000000.), ((double)length)/1000000.);
   } else {
      printf("\t* core %lu: nothing done during %lu us\n",
            tn->assigned_core,
            length
            );
   }


   // Last rendez-vous
   rdv(tn->thread_no);

   //free(pdata);
   return NULL;
}

void usage(char * app_name) {
   unsigned long nb_plugins = sizeof(plugins) / sizeof(memory_bench_plugin_t);

   fprintf(stderr, "Usage: %s -t <plugin number> -c <list of cores> [-m <memory node>] [ -l <size>[K|M|G] | -g <size>[K|M|G] ] [-T <time in cycles>]\n", app_name);
   fprintf(stderr, "\t-t: plugin number. Available plugins:\n");
   int i = 0;
   for (i = 0; i < nb_plugins; i++) {
      printf("\t\t%d - %s\n", i, plugins[i].name);
   }

   fprintf(stderr, "\t-c: list of cores separated by commas\n");
   fprintf(stderr, "\t-m: memory node to benchmark or -1 for local allocation (default = -1)\n");
   fprintf(stderr, "\t-f: run a spinloop on the first core of unused nodes\n");
   fprintf(stderr, "\t-l: memory size to benchmark (per thread)\n");
   fprintf(stderr, "\t-g: memory size to benchmark (total)\n");
   fprintf(stderr, "\t-T: specify manually the benchmark duration (in ms)\n");
   fprintf(stderr, "\t-h: display usage\n");
   exit(EXIT_FAILURE);
}

static uint64_t parse_size (char * size) {
   int length = strlen(size);
   int factor = 1;
   if(size[length-1] == 'K') {
      factor = 1024;
   }
   else if (size[length-1] == 'M') {
      factor = 1024*1024;
   }
   else if (size[length-1] == 'G') {
      factor = 1024*1024*1024;
   }

   //printf("Factor is %d (%c)\n", factor, size[length-1]);
   size[length-1] = 0;
   return (uint64_t) atoi(size) * factor;
}

int main(int argc, char **argv){

   ndies = numa_num_configured_nodes();
   int ncores = get_nprocs();

   int current_buf_size = ncores;
   int * cores = (int*) malloc(current_buf_size * sizeof(int));
   int fake_loop = 0;
   nthreads = 0;

   /** Parsing options **/
   opterr = 0;
   int c;

   uint64_t total_memory_to_alloc = 0;
   uint64_t per_thread_memory_to_alloc = 0;

   while ((c = getopt(argc, argv, "fhc:m:t:g:l:T:")) != -1) {
      char delims[] = ",";
      char * result = NULL;

      switch (c) {
         case 'c':
            result = strtok( optarg, delims );
            while( result != NULL ) {
                if(++nthreads > current_buf_size) {
                   current_buf_size += ncores;
                   cores = realloc (cores, current_buf_size * sizeof(int));
                   assert(cores);
                }
                cores[nthreads-1] = atoi(result);
                if(cores[nthreads-1] < 0 || cores[nthreads-1] >= ncores){
                   fprintf(stderr, "%d is not a valid core number. Must be comprised between 0 and %d", cores[nthreads-1], ncores-1);
                   exit(EXIT_FAILURE);
                }

                result = strtok( NULL, delims );
            }
            break;
         case 'm':
            die_on_which_to_alloc = atoi(optarg);
            if(die_on_which_to_alloc < -1 || die_on_which_to_alloc >= (int) ndies){
               die("%d is not a valid memory allocation policy (max die = %d || -1)", die_on_which_to_alloc, ndies);
            }
            break;
         case 't':
            choosed_plugin = atoi(optarg);
            unsigned long nb_plugins = sizeof(plugins) / sizeof(memory_bench_plugin_t);
            if(choosed_plugin < 0 || choosed_plugin >= nb_plugins){
               die("%d is not a valid plugin number", choosed_plugin);
            }
            break;
         case 'g':
            total_memory_to_alloc = parse_size(optarg);
            break;
         case 'l':
            per_thread_memory_to_alloc = parse_size(optarg);
            break;
         case 'T':
            bench_time = atoll(optarg);
            break;
         case 'f':
            fake_loop = 1;
            break;
         case 'h':
            usage(argv[0]);
         case '?':
            if (optopt == 'c' || optopt == 'm' ||  optopt == 't' || optopt == 'g' || optopt == 'l' || optopt == 'i')
               fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
               fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
               fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            usage(argv[0]);
         default:
            usage(argv[0]);
      }
   }

   if(nthreads == 0 || choosed_plugin < 0){
      fprintf(stderr, "Some options are not properly filled (%s)\n\n", nthreads == 0?"Cores are not specified":"No plugin chosen");
      usage(argv[0]);
   }

   if(total_memory_to_alloc && per_thread_memory_to_alloc) {
      fprintf(stderr, "You must choose between -g and -l !\n");
      usage(argv[0]);
   }
   else if (total_memory_to_alloc > 0) {
      memory_size = total_memory_to_alloc / nthreads;
   }
   else if(per_thread_memory_to_alloc > 0) {
      memory_size = per_thread_memory_to_alloc;
   }

   if(memory_size <= 0) {
      fprintf(stderr, "Memory size not specified or not consistent. Using the builtin memory size...\n");
      memory_size = DEFAULT_MEMORY_BENCH_SIZE_TO_BENCH;
   }

   printf("Bench parameters\n");
   printf("\t* Required %d threads\n", nthreads);
   printf("\t* Memory allocation policy: %d (%s)\n", die_on_which_to_alloc, (die_on_which_to_alloc == -1) ? "Local" : "On die");
   printf("\t* Memory size to bench: %llu bytes\n", (unsigned long long) memory_size);
   printf("\t* Benchmark time: %lus\n", (unsigned long)bench_time);

   /** Scale the bench_time in cycles */
   bench_time = bench_time * get_cpu_freq(); 

   /** Allocation stuff **/
   nb_bytes_processed = calloc(nthreads, sizeof(uint64_t));
   pthread_t * threads = malloc(nthreads * sizeof(pthread_t));
   assert(nb_bytes_processed);
   assert(threads);

   /** Seting the memory policy **/
   if(die_on_which_to_alloc == -1) {
      printf("Setting the local memory policy\n");
      assert(set_mempolicy(MPOL_PREFERRED, NULL, 0) == 0);
   }
   else {
      long unsigned int nodes = 0;
      nodes += 1 << die_on_which_to_alloc;

      printf("Setting the BIND memory policy (mask = %lx)\n", nodes);
      assert(set_mempolicy(MPOL_BIND, &nodes, sizeof(long unsigned int)) == 0);
   }

   assert(pthread_mutex_init(&mutex, NULL) == 0);
   assert(pthread_cond_init(&go_to_work, NULL) == 0);

   int* dies_assigned = calloc(nb_dies, sizeof(int));

   /* 1. Create a thread for each core specified on the command line */
   struct thread_data * pdata = malloc(sizeof(struct thread_data));
   assert(pdata);

   int i = 0;
   for(i = 0; i < nthreads; i++) {
      pdata->assigned_core = cores[i];
      pdata->thread_no = i;
      pdata->do_work = 1;

      dies_assigned[numa_node_of_cpu(cores[i])] = 1;

      assert(pthread_create(&threads[i], NULL, thread_loop, (void*)pdata) == 0);
      pdata = malloc(sizeof(struct thread_data));
      assert(pdata);
   }

   /* 2. Create a thread on each die which currently has no thread (idle threads: do_work == 0) */
   int fake_thread_no = nthreads;
   for (i = 0; fake_loop && i < ndies; i++) {
      if (!dies_assigned[i]) {
         struct bitmask * bm = numa_allocate_cpumask();
         numa_node_to_cpus(i, bm);
         int core = -1;
         int j = 0;
         for (j = 0; j < ncores; j++) {
            if (numa_bitmask_isbitset(bm, j)) {
               core = j;
               break;
            }
         }

         pdata->assigned_core = core;
         pdata->thread_no = fake_thread_no++;
         pdata->do_work = 0;
         assert(pthread_create(&threads[i], NULL, thread_loop, (void*)pdata) == 0);
         pdata = malloc(sizeof(struct thread_data));
      }
   }

   /** Join **/
   for( i = 0; i < nthreads; i++) {
      pthread_join(threads[i], NULL);
   }

   return EXIT_SUCCESS;
}
