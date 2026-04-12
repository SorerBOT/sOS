#ifndef MATH_EXTENDED_H
#define MATH_EXTENDED_H

#include <types.h>
#include <math.h>


qword math_extended_round_up_to_power_of_two(qword x);
qword math_extended_round_up_to_log_two(qword x);
qword math_extended_right_smear(qword x);
qword math_extended_popcount(qword x);

#endif /* MATH_EXTENDED_H */
