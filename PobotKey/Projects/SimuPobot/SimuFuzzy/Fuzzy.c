/*
	Application name:	FUzzy Development and Generation Environment (FUDGE) Version V1.02
	File name:			Fuzzy.c
	Written by:			Alex DeCastro & Jason Spielman

	Copyright Motorola 1994
	SCALE	1
*/

#include <stdio.h>
#include "Fuzzy.h"

int	num_inputs = 4;
int	num_outputs = 2;
int	num_rules = 12;

int	num_input_mfs[4] = { 4, 5, 3, 3 };

struct	In	Inputs[] =
{
	{ 0.000000, 3600.000000 },
	{ -180.000000, 180.000000 },
	{ 0.000000, 1000.000000 },
	{ -90.000000, 90.000000 }
};

float	inmem_points[4][7][4] =
{
	{
		{ 0.000000, 0.000000, 5.000000, 50.000000 },
		{ 10.000000, 50.000000, 500.000000, 1000.000000 },
		{ 500.000000, 1000.000000, 3000.000000, 3600.000000 },
		{ 3000.000000, 3600.000000, 3600.000000, 3600.000000 }
	},
	{
		{ -90.000000, 0.000000, 0.000000, 90.000000 },
		{ -180.000000, -90.000000, -90.000000, 0.000000 },
		{ 0.000000, 90.000000, 90.000000, 180.000000 },
		{ -180.000000, -180.000000, -180.000000, -90.000000 },
		{ 90.000000, 180.000000, 180.000000, 180.000000 }
	},
	{
		{ 0.000000, 0.000000, 150.000000, 300.000000 },
		{ 150.000000, 300.000000, 450.000000, 600.000000 },
		{ 450.000000, 600.000000, 1000.000000, 1000.000000 }
	},
	{
		{ -90.000000, -90.000000, -40.000000, -20.000000 },
		{ -40.000000, -20.000000, 20.000000, 40.000000 },
		{ 20.000000, 40.000000, 90.000000, 90.000000 }
	}
};
int	num_output_mfs[2] = { 5, 5 };

struct	Out	Outputs[] =
{
	{ -250.000000, 250.000000 },
	{ -250.000000, 250.000000 }
};

float	outmem_points[2][7][4] =
{
	{
		{ 0.000000 },
		{ 60.000000 },
		{ 250.000000 },
		{ -60.000000 },
		{ -250.000000 }
	},	{
		{ 0.000000 },
		{ 60.000000 },
		{ 250.000000 },
		{ -60.000000 },
		{ -250.000000 }
	}
};
float	crisp_outputs[2] = { 0, 0};

int	num_rule_ants[12] = { 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
int	num_rule_cons[12] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

struct	Rule	Rules[12] =
{
	{ { 0x00 }, { 0x80, 0x81 } },
	{ { 0x10, 0x01 }, { 0x90, 0x91 } },
	{ { 0x10, 0x09 }, { 0x80, 0x91 } },
	{ { 0x10, 0x11 }, { 0x90, 0x81 } },
	{ { 0x10, 0x19 }, { 0xa0, 0x91 } },
	{ { 0x10, 0x21 }, { 0x90, 0xa1 } },
	{ { 0x08, 0x01 }, { 0x88, 0x89 } },
	{ { 0x08, 0x09 }, { 0x80, 0x89 } },
	{ { 0x08, 0x11 }, { 0x88, 0x81 } },
	{ { 0x08, 0x19 }, { 0x98, 0x89 } },
	{ { 0x08, 0x21 }, { 0x88, 0x99 } },
	{ { 0x02, 0x0b }, { 0xa0, 0xa1 } }
};

void fuzzy_step(float *crisp_inputs, float *crisp_outputs)
{
	int             in_index,rule_index,out_index;
	float   in_val;
	for (in_index = 0;in_index < num_inputs;in_index++)
	{
		fuzzify_input(in_index,crisp_inputs[in_index]);
	}
	for (rule_index = 0;rule_index < num_rules;rule_index++)
	{
		eval_rule(rule_index);
	}
	for (out_index = 0;out_index < num_outputs;out_index++)
	{
		crisp_outputs[out_index] = defuzzify_output(out_index, crisp_inputs);
		if (TRACE) printf("crisp_output[%d] = %f\n", out_index, crisp_outputs[out_index]);
	}
}
void fuzzify_input(int in_index,float in_val)
{
	int i;
	if (TRACE) printf("Fuzzify: input #%d crisp value %f\n", in_index, in_val);
	for (i = 0;i < num_input_mfs[in_index];i++)
	{
		fuzzy_inputs[in_index][i] = get_membership_value(in_index,i,in_val);
		if (TRACE) printf("Membership function #%d grade %f\n", i, fuzzy_inputs[in_index][i]);
	}
}
float get_membership_value(int in_index,int mf_index,float in_val)
{
	if (in_val < inmem_points[in_index][mf_index][0]) return 0;
	if (in_val > inmem_points[in_index][mf_index][3]) return 0;
	if (in_val <= inmem_points[in_index][mf_index][1])
	{
		if (inmem_points[in_index][mf_index][0] == inmem_points[in_index][mf_index][1])
			return 1;
		else
			return ((in_val - inmem_points[in_index][mf_index][0]) /
					(inmem_points[in_index][mf_index][1] - inmem_points[in_index][mf_index][0]));
	}
	if (in_val >= inmem_points[in_index][mf_index][2])
	{
		if (inmem_points[in_index][mf_index][2] == inmem_points[in_index][mf_index][3])
			return 1;
		else
			return ((inmem_points[in_index][mf_index][3] - in_val) /
					(inmem_points[in_index][mf_index][3] - inmem_points[in_index][mf_index][2]));
	}
	return 1;
}
void eval_rule(int rule_index)
{
	int             in_index,out_index,mf_index,ant_index,con_index;
	int     val;
	float   rule_strength = 1;
	for     (ant_index = 0;ant_index < num_rule_ants[rule_index];ant_index++)
	{
		val = Rules[rule_index].antecedent[ant_index];
		in_index = (val & 0x07);
		mf_index = ((val & 0x38) >> 3);
		rule_strength = MIN(rule_strength,fuzzy_inputs[in_index][mf_index]);
	}
	rule_strengths[rule_index] = rule_strength;
	if (TRACE) printf("Rule  #%d strength %f\n", rule_index, rule_strength);
	for (con_index = 0;con_index < num_rule_cons[rule_index];con_index++)
	{
		val = Rules[rule_index].consequent[con_index];
		out_index = (val & 0x03);
		mf_index = ((val & 0x38) >> 3);
		fuzzy_outputs[out_index][mf_index] = MAX(fuzzy_outputs[out_index][mf_index],
			rule_strengths[rule_index]);
	}
}
float defuzzify_output(int out_index,float *inputs)
{
	float           summ = 0;
	float           product = 0;
	float           temp1,temp2;
	int             mf_index,in_index;
	if (TRACE) printf("Defuzzify: output #%d\n", out_index);
	for (mf_index = 0;mf_index < num_output_mfs[out_index];mf_index++)
	{
		temp1 = fuzzy_outputs[out_index][mf_index];
		temp2 = outmem_points[out_index][mf_index][0];
		summ = summ + temp1;
		product = product + (temp1 * temp2);
		if (TRACE) printf("Membership function #%d grade %f\n", mf_index, fuzzy_outputs[out_index][mf_index]);
		fuzzy_outputs[out_index][mf_index] = 0;
	}
	if (summ > 0)
	{
		crisp_outputs[out_index] = product / summ;
		return crisp_outputs[out_index];
	}
	else
	{
		if (NO_RULES){
		printf("No rules fire for:\n");
			for (in_index = 0;in_index < num_inputs;in_index++)
			    printf("Input #%d=%f ", in_index,inputs[in_index]);
			    printf("\n");}
		return crisp_outputs[out_index];
	}
}
