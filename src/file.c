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

#include <src/json.h>
#include <src/file.h>
#include <src/fields.h>

match_t *load_match_file(char *path, char *team)
{   match_t *ret = match_new();
	json_t *json;
	json_error_t error;

	json = json_load_file(path, 0, &error);
	if(!json) {
		fprintf(stderr, "ERROR: %s\n", error.text);

		return NULL;
	}

	if(team && (atoi(team) != json_integer_value(json_object_get(json, "team")))) {
		fprintf(stderr, "ERROR: Teams do not match. (\"%s\" and \"%lli\")\n",
		        team,
		        json_integer_value(json_object_get(json, "team")));

		return NULL;
	}

	match_set_from_data(ret,
	                    string_to_position((char *)json_string_value(json_object_get(json, "position"))),
	                    (char *)json_string_value(json_object_get(json, "round")),
	                    string_to_auto_shot((char *)json_string_value(json_object_get(json, "auto_shot"))),
	                    yes_no_string_to_boolean((char *)json_string_value(json_object_get(json, "auto_hot"))),
	                    yes_no_string_to_boolean((char *)json_string_value(json_object_get(json, "auto_mobility"))),
	                    json_integer_value(json_object_get(json, "high_goals_scored")),
	                    json_integer_value(json_object_get(json, "high_goals_attempted")),
	                    json_integer_value(json_object_get(json, "low_goals_scored")),
	                    json_integer_value(json_object_get(json, "low_goals_attempted")),
	                    json_integer_value(json_object_get(json, "trusses_scored")),
	                    json_integer_value(json_object_get(json, "trusses_attempted")),
	                    json_integer_value(json_object_get(json, "catches_scored")),
	                    json_integer_value(json_object_get(json, "catches_attempted")),
	                    json_integer_value(json_object_get(json, "passes_scored")),
	                    json_integer_value(json_object_get(json, "passes_attempted")),
	                    json_integer_value(json_object_get(json, "receives_scored")),
	                    json_integer_value(json_object_get(json, "receives_attempted")),
	                    json_integer_value(json_object_get(json, "blocks_scored")),
	                    json_integer_value(json_object_get(json, "blocks_attempted")),
	                    (char *)json_string_value(json_object_get(json, "notes")));

	return ret;
}

team_t *load_team_dir(char *dir_path, char *team)
{	team_t *ret = team_new();
	match_t **matches = malloc(sizeof(match_t *) * 4096);
	int i = 0;
	tinydir_dir dir;
	tinydir_open(&dir, dir_path);

	while(dir.has_next) {
		tinydir_file file;
		match_t *match;

		tinydir_readfile(&dir, &file);

		if(!file.is_dir) { /* not a directory (assuming therefore regular file) */

			char *dot = strrchr(file.name, '.');
			if(dot && !strcmp(dot, ".json")) { /* ends w/ .json */

				char *fullpath = malloc((strlen(dir_path) +
				                         strlen(team) +
				                         strlen(file.name))
				                        * sizeof(char));

				strcpy(fullpath, dir_path);
				strcat(fullpath, "/");
				strcat(fullpath, file.name);

				if(match = load_match_file(fullpath, team)) { /* make sure there were no errors */
					matches[i] = match; /* add to matches */
					i++;
				}
			}
		}

		tinydir_next(&dir);
	}

	tinydir_close(&dir);

	if(i == 0) return NULL;
	else return team_new_from_data((unsigned int)atoi(team), "", matches, i);
}

int load_dot_scoutify(char *dsf_path)
{
	int i = 0;
	tinydir_dir dir;
	tinydir_open(&dir, dsf_path);

	while(dir.has_next) {
		tinydir_file file;
		team_t *team;
		tinydir_readfile(&dir, &file);

		if(file.is_dir && /* a directory, assuming to be a team dir */
		   (strcmp(".", file.name) && strcmp("..", file.name))) { /* is not . or .. */

			char *fullpath = malloc((strlen(dsf_path) +
			                         strlen(file.name))
			                        * sizeof(char));

			strcpy(fullpath, dsf_path);
			strcat(fullpath, "/");
			strcat(fullpath, file.name);

			if(team = load_team_dir(fullpath, file.name)) { /* make sure there were matches */
				database_add_team(team); /* add to db */
				i++;
			}
		}

		tinydir_next(&dir);
	}

	tinydir_close(&dir);

	if(i == 0) return 1;
	else return 0;
}

int write_db_to_file(char *path)
{
	json_dump_file(make_json_from_db(), path, 0);
	return 0;
}

int populate_db_from_file(char *path)
{
	json_t *json;
	json_error_t error;

	json = json_load_file(path, 0, &error);

	if(!json) {
		fprintf(stderr, "ERROR: %s\n", error.text);
		return -1;
	}

	populate_db_from_json(json);

	return 0;
}

int dump_database_to_csv_file(char *opath)
{
	/* open the path given for writing */
	FILE *file = fopen(opath, "wb");

	if(file == NULL) { /* that is, if fopen didn't set it to an actual memory address */
		fprintf(stderr, "OH NOES, DUMPFILE [%s] IS NULL!\n", opath);
		return 1;
	}

	team_t *curteam;

	/* print the column headers to the CSV file */
	fprintf(file, "\"dbindex\", \"number\", \"value\", \"name\"\n");

	for(int i = 0; curteam = database_get_nth_element(i); i += 1) {
		fprintf(file, "\"%d\", \"%u\", \"%f\", \"%s\"\n", i, curteam->number, curteam->value, curteam->name);
	}

	fclose(file);
	return 0;
}
