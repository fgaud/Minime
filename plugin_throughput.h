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

/*
* This plugin uses the ASM code of bandwidth (0.16), available here : http://zsmith.co/bandwidth.html
* This program is free software, published under the terms of the GNU General 
* Public License either version 2 of the License, or any later version.
*/


#ifndef PLUGIN_THROUGHPUT_H_
#define PLUGIN_THROUGHPUT_H_

#include <stdint.h>


uint64_t bench_throughtput(uint64_t* memory_to_access, uint64_t memory_size, uint32_t nb_iterations, uint32_t thread_no);

#endif /* PLUGIN_THROUGHPUT_H_ */
