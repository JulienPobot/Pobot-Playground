/*
	Application name:       FUzzy Development and Generation Environment (FUDGE) Version V1.00
	File name:                      Fuzzy.h
	Written by:                     Alex DeCastro & Jason Spielman

	Copyright Motorola 1994
*/

#define TRACE          0      /*set to display fuzzy parameters */
#define NO_RULES       1      /*set to display inputs when no rules fire*/
#define MIN(A,B)       (A < B) ? A : B
#define MAX(A,B)       (A > B) ? A : B

struct In {
	float           min;
	float           max;
};
struct Out {
	float           min;
	float           max;
};
struct Rule {
	int             antecedent[8];
	int             consequent[8];
};

float           fuzzy_inputs[8][8];
float           fuzzy_outputs[4][8];
float           rule_strengths[64];

void    fuzzy_step(float *, float *);
void    fuzzify_input(int, float);
float   get_membership_value(int, int, float);
void    eval_rule(int);
float   defuzzify_output(int, float *);
