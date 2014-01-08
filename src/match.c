/*
    Copyright Christopher Cooper and Kristofer Rye 2013

    This file is part of scoutify-merger.

    scoutify-merger is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    scoutify-merger is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with scoutify-merger.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <src/match.h>

/* Simply creates, allocates, and returns an empty match structure (pointers inside may not be initialized!) */
match_t *match_new()
{ match_t *ret = malloc(sizeof(match_t));
	return ret;
}

/* Creates a new match struct and populates it with the arguments given as the data */
match_t *match_new_from_data(position_t position,
                             char round[16],
                             int auto_shots,
                             int auto_scores,
                             int auto_goal,
                             int teleop_shots,
                             int teleop_scores,
                             int teleop_goal,
                             int climb_tier,
                             char notes[256])
{ match_t *ret = match_new();

	/* TODO: add validations, perhaps detect if stuff isn't allocated and call functions to allocate it [matches ptr] */
	
	ret->position = position;
	strcpy(ret->round, round);
	ret->auto_shots = auto_shots;
	ret->auto_scores = auto_scores;
	ret->auto_goal = auto_goal;
	ret->teleop_shots = teleop_shots;
	ret->teleop_scores = teleop_scores;
	ret->teleop_goal = teleop_goal;
	ret->climb_tier = climb_tier;
	strcpy(ret->notes, notes);

	return ret;
}

int match_set_from_data(match_t *in,
                        position_t position,
                        char round[16],
                        int auto_shots,
                        int auto_scores,
                        int auto_goal,
                        int teleop_shots,
                        int teleop_scores,
                        int teleop_goal,
                        int climb_tier,
                        char notes[256])
{
	in->position = position;
	strcpy(in->round, round);
	in->auto_shots = auto_shots;
	in->auto_scores = auto_scores;
	in->auto_goal = auto_goal;
	in->teleop_shots = teleop_shots;
	in->teleop_scores = teleop_scores;
	in->teleop_goal = teleop_goal;
	in->climb_tier = climb_tier;
	strcpy(in->notes, notes);

	return 0;
}

int match_free();

int print_match(match_t *match)
{
	printf("position_num: %s\n", position_to_string(match->position));
	printf("round: %s\n", match->round);
	printf("auto_shots: %i\n", match->auto_shots);
	printf("auto_scores: %i\n", match->auto_scores);
	printf("auto_goal: %i\n", match->auto_goal);
	printf("teleop_shots: %i\n", match->teleop_shots);
	printf("teleop_scores: %i\n", match->teleop_scores);
	printf("teleop_goal: %i\n", match->teleop_goal);
	printf("climb_tier: %i\n", match->climb_tier);
	printf("notes: %s\n", match->notes);

}
