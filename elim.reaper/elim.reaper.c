/*
 * Name:    elim.reaper
 *
 * Purpose: LSF ELIM plugin to remove user login processes from 
 *          LSF compute hosts for users who are not part of an 
 *          active job, or that have lingered beyond the
 *          linger timeout as defined by the lsf.reaper configuration
 *          file in the $LSF_ENVDIR.
 *
 * Author:  iarry Adams <adamsla@us.ibm.com>
 *
 */

#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <lsf/lsbatch.h>
#include <lsf/lsf.h>

#define VERSION "1.1"
#define ELIM_ABORT_VALUE 99
#define STRMATCH(a,b)   (strcmp((a),(b)) == 0)
#define STRIMATCH(a,b)  (strcasecmp((a),(b)) == 0)

int lingerTime = 300;
int debug = FALSE;
int mySleep = 60;
char excludedHosts[2048];

typedef struct user_struct {
	char user[40];
	int pid;
	int lingerTime;
} user_struct_t;

typedef struct admins {
	char admin[40];
} admins_t;

static void display_help(int only_version);
void load_lsf_reaper();
int is_excluded_host(char *hostname);
char *ltrim(char *);
char *rtrim(char *);
char *trim(char *);

int main(int argc, char **argv) {
	int i, j;
	struct utsname name;
	char kill_command[60];

	/* array structures holding users */
	user_struct_t *loginUsers;
	user_struct_t *jobUsers;

	/* static variables of the max of each type of user */
	int maxLogins   = 200;
	int maxJobUsers = 1000;

	/* booleans telling us if the calls were good */
	int validLogins = FALSE;
	int validJobs   = FALSE;

	for (argv++; *argv; argv++) {
		char    *arg = *argv;
		char    *opt = strchr(arg, '=');

		if (opt) *opt++ = '\0';

		if (STRMATCH(arg, "-s") ||
			STRMATCH(arg, "--sleep")) {
			mySleep = atoi(opt);
		} else if (STRMATCH(arg, "-h") ||
			STRMATCH(arg, "-H") ||
			STRMATCH(arg, "--help")) {
			display_help(FALSE);
			exit(0);
		} else if (STRMATCH(arg, "-v") ||
			STRMATCH(arg, "-V") ||
			STRMATCH(arg, "--version")) {
			display_help(TRUE);
			exit(0);
		} else if (STRMATCH(arg, "-d") ||
			STRMATCH(arg, "-D") ||
			STRMATCH(arg, "--debug")) {
			debug = TRUE;
		} else {
			printf("ERROR: Unknown option %s with value %s\n", arg, opt);
			display_help(FALSE);
			exit(ELIM_ABORT_VALUE);
		}
	}

	/* get hostname where we are running */
	if (uname(&name) == -1) {
		syslog(LOG_ERR, "elim.reaper: couldn't get hostname");
		exit(ELIM_ABORT_VALUE);
	}

	if (debug) {
		printf("elim.reaper: Current hostname = %s\n", name.nodename);
	}

	loginUsers = (user_struct_t *) calloc(maxLogins, sizeof(user_struct_t));;
	jobUsers   = (user_struct_t *) calloc(maxJobUsers, sizeof(user_struct_t));;
	memset(loginUsers, 0, sizeof(loginUsers));
	memset(jobUsers, 0, sizeof(jobUsers));

	load_lsf_reaper();

	if (is_excluded_host(name.nodename)) {
		exit(ELIM_ABORT_VALUE);
	}
	
	if (valid_lsf_setup()) {
		if (debug) {
			printf("elim.reaper: LSF setup is valid\n");
		}

		while(TRUE) {
			/* get the information on jubs and users */
			validLogins = get_login_sessions(loginUsers, maxLogins);
			validJobs   = get_job_users(jobUsers, maxJobUsers, name.nodename);

			if (validLogins) {
				i = 0;

				while(i < validLogins) {
					if (loginUsers[i].user != '\0') {
						if (!is_lsf_admin(loginUsers[i].user)) {
							j = 0;

							if (validJobs > 0) {
								while(j < validJobs) {
									if (jobUsers[j].user != NULL) {
										if (strcmp(loginUsers[i].user, jobUsers[j].user) == 0) {
											if (debug) {
												printf("elim.reaper: Matching job found for (%s)\n", loginUsers[i].user);
											}

											break;
										}
									} else {
										if (debug) {
											printf("elim.reaper: killing login process group for user (%s) with pid (%d) due to no job.\n", loginUsers[i].user, loginUsers[i].pid);
										}

										syslog(LOG_NOTICE, "elim.reaper: killing login process for user (%s) with pid (%d) due to no job.", loginUsers[i].user, loginUsers[i].pid);
										killpg(loginUsers[i].pid, SIGKILL);

										break;
									}

									j++;
								}
							} else {
								if (debug) {
									printf("elim.reaper: killing login process group for user (%s) with pid (%d) due to no job.\n", loginUsers[i].user, loginUsers[i].pid);
								}

								syslog(LOG_NOTICE, "elim.reaper: killing login process group for user (%s) with pid (%d) due to no job.", loginUsers[i].user, loginUsers[i].pid);
								killpg(loginUsers[i].pid, SIGKILL);
							}
						} else if (debug) {
							printf("elim.reaper: User (%s) is an LSF Admin skipping\n", loginUsers[i].user);
						}

						i++;
					} else {
						if (debug) {
							printf("elim.reaper: End of login users reached.\n");
						}

						break;
					}
				}
			}

			memset(loginUsers, 0, sizeof(loginUsers));
			memset(jobUsers, 0, sizeof(jobUsers));

			if (debug) {
				printf("elim.reaper: Sleeping for %d seconds.\n", mySleep);
			}

			printf("1 reaper 1\n");

			sleep(mySleep);
		}
	} 

	exit(ELIM_ABORT_VALUE);
}

int is_excluded_host(char *hostname) {
	if (excludedHosts == NULL) {
		return FALSE;
	}
	
	if (strcasestr(excludedHosts, hostname)) {
		if (debug) {
			printf("elim.reaper: host [%s] is an excluded host of [%s].\n", hostname, excludedHosts);
		}

		return TRUE;
	} else {
		if (debug) {
			printf("elim.reaper: host [%s] is not an excluded host of [%s].\n", hostname, excludedHosts);
		}

		return FALSE;
	}
}

void load_lsf_reaper() {
	const char *envdir = getenv("LSF_ENVDIR");
	char reaperfile[255];
	FILE * fp;
	int i, j;
	char * line = NULL;
	size_t len  = 0;
	ssize_t read;
	char variable[60];
	char value[2048];

	snprintf(reaperfile, sizeof(reaperfile), "%s/%s", envdir, "lsf.reaper");

	if (file_exists(reaperfile)) {
		i = 0;

		if (debug) {
			printf("elim.reaper: Starting reading the lsf.reaper file.\n");
		}

		fp = fopen(reaperfile, "r");

		if (fp == NULL) {
			if (debug) {
				printf("elim.reaper: WARNING: Unable to open the lsf.reaper file.\n");
			}
		}

		char *p;

		while ((read = getline(&line, &len, fp)) != -1) {
			line = trim(line);

			if (debug) {
				printf("elim.reaper: LINE: %s\n", line);
			}

			j = 0;
			p = strtok(line, "=");

			while (p != NULL) {
				if (j == 0) {
					snprintf(variable, 40, "%s", trim(p));
					if (debug) {
						printf("elim.reaper: VARIABLE: (%s)\n", variable);
					}
				} else if (j == 1) {
					snprintf(value, 40, "%s", trim(p));
					if (debug) {
						printf("elim.reaper: VALUE: (%s)\n", value);
					}
				} else {
					break;
				}

				j++;

				p = strtok (NULL, "=");
			}

			if (j > 0) {
				if (STRMATCH(variable, "LSF_SLEEP_TIME")) {
					mySleep = atoi(value);
				} else if (STRMATCH(variable, "LSF_LINGER_TIME")) {
					lingerTime = atoi(value);
				} else if (STRMATCH(variable, "LSF_EXCLUDED_HOSTS")) {
					snprintf(excludedHosts, sizeof(excludedHosts), "%s", value);
				}
			}

			i++;
		}
	}
}

int valid_lsf_setup() {
	struct    parameterInfo  *paramInfo;

	if (lsb_init("elim.reaper") < 0) {
		return FALSE;
	}

	if (!(paramInfo = lsb_parameterinfo (NULL, NULL, 0))) {
		return FALSE;
	}

	if (!paramInfo->newjobRefresh) {
		printf("elim.reaper: FATAL: Unable to utilize when NEWJOB_REFRESH=N in lsb.params.\n");
		printf("elim.reaper: Run './elim.reaper --help' for more information.\n");
		return FALSE;
	}

	if (!paramInfo->jobIncludePostproc) {
		if (lingerTime <= 0) {
			printf("elim.reaper: FATAL: Unable to use when JOB_INCLUDE_POSTPROC=N in lsb.params and lingerTime=0.");
			printf("elim.reaper: Run './elim.reaper --help' for more information.\n");
			return FALSE;
		} else if (lingerTime <= 600) {
			printf("elim.reaper: WARNING: JOB_INCLUDE_POSTPROC=N in lsb.params and lingerTime is %d\n", lingerTime);
			printf("elim.reaper: Run './elim.reaper --help' for more information.\n");
		}
	}

	return TRUE;
}

/*! \fn trim()
 *  \brief removes leading and trailing blanks, tabs, line feeds and
 *         carriage returns from a string.
 *
 *  \return the trimmed string.
 */
char *trim(char *str) {
	return ltrim(rtrim(str));
}

/*! \fn rtrim()
 *  \brief removes trailing blanks, tabs, line feeds and carriage returns
 *         from a string.
 *
 *  \return the trimmed string.
 */
char *rtrim(char *str) {
	char    *end;
	char    *trim = "\"' \t\n\r";

	if (!str) return NULL;

	end = str + strlen(str);

	while (end-- > str) {
		if (!strchr(trim, *end)) return str;

		*end = 0;
	}

	return str;
}

/*! \fn ltrim()
 *  \brief removes leading blanks, tabs, line feeds and carriage returns
 *         from a string.
 *
 *  \return the trimmed string.
 */
char *ltrim(char *str) {
	char    *trim = "\"' \t\n\r";

	if (!str) return NULL;

	while (*str) {
		if (!strchr(trim, *str)) return str;

		++str;
	}

	return str;
}

int get_login_sessions(user_struct_t *users, int maxLogins) {
	int i, j;
	FILE * fp;
	int logins = 0;

	char command[128] = "who -u | grep -v root | awk '{ printf(\"%s %s\\n\", $1, $6) }'";

	char * line = NULL;
	size_t len  = 0;
	ssize_t read;

	i = 0;

	if (debug) {
		printf("elim.reaper: Starting to get login users\n");
	}

	fp = popen((char *) command, "r");

	if (fp == NULL) {
		if (debug) {
			printf("elim.reaper: WARNING: Unable to call the 'who' command\n");
		}

		return FALSE;
	}

	char *p;

	while ((read = getline(&line, &len, fp)) != -1) {
		line = trim(line);

		if (debug) {
			printf("elim.reaper: LINE: %s\n", line);
		}

		j = 0;
		p = strtok(line, " ");

		while (p != NULL) {
			if (j == 0) {
				if (debug) {
					printf("elim.reaper: USER: (%s)\n", p);
				}

				snprintf(users[i].user, 40, "%s", p);
			} else if (j == 1) {
				if (debug) {
					printf("elim.reaper: PID:  (%s)\n", p);
				}

				users[i].pid = atoi(p);

				logins++;
			} else {
				break;
			}

			j++;

			p = strtok (NULL, " ");
		}

		i++;

		if (i > maxLogins) {
			syslog(LOG_WARNING, "WARNING: Maximum number of logins (%d) Reached\n", maxLogins);
			break;
		}
	}

	pclose(fp);

	return logins;
}

/*! \fn int file_exists(const char *filename)
 *  \brief checks for the existance of a file.
 *  \param *filename the name of the file to check for.
 *
 *  \return TRUE if found FALSE if not.
 *
 */
int file_exists(const char *filename) {
	struct stat file_stat;

	if (stat(filename, &file_stat)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

/*! \fn int get_job_users(char *users, int maxJobUsers, char *hostname)
 *  \brief returns a list of users with jobs on the host
 *  \param *users an empty array passed to the function.
 *  \param *maxJobUsers an integer with the size of the array
 *  \param *hostname the host that should be searched for jobs
 *
 *  \return TRUE if found FALSE if not.
 *
 */
int get_job_users(user_struct_t *users, int maxJobUsers, char *hostname) {
	int options = ALL_JOB;
	int jobs;
	int nowTime = (int)time(NULL);
	int i;
	int more;
	char * user = "all";

	struct jobInfoEnt *job;

	int retries = 3;
	int count = 0;
	int error_count = 0;
	int endTime = 0;

	if (debug) {
		printf("elim.reaper: Starting to get LSF users\n");
	}

	/* initializing LSF */
	if (debug) {
		syslog(LOG_NOTICE, "elim.reaper: initializing LSF");
	}

	if (lsb_init("elim.reaper") < 0) {
		syslog(LOG_NOTICE, "elim.reaper: lsb_init() failed allowing login");
		return FALSE;
	}

	if (debug) {
		syslog(LOG_NOTICE, "elim.reaper: checking jobs for LSF hostname=%s", hostname);
	}

	jobs = lsb_openjobinfo(0, NULL, user, NULL, hostname, options);

	if (debug) {
		printf("elim.reaper: Found (%d) User jobs\n", jobs);
	}

	if (jobs < 1) {
		lsb_closejobinfo();
		return jobs;
	} else {
		while(TRUE) {
			retry:

			job = lsb_readjobinfo(&more);

			if (job == NULL) {
				if (error_count < 20) {
					switch(lserrno) {
					case LSE_NO_ERR:
						break;
					default:
						error_count++;
						usleep(100000);
						goto retry;
					}
				} else {
					syslog(LOG_ERR, "elim.reaper: error unable to read job data after 20 tries!");
					return jobs;
				}
			}

			endTime = (long int) job->endTime;

			if (debug) {
				printf("elim.reaper: Found Job (%ul) with endTime (%i), curTime (%i), lingerTime (%i)\n", LSB_ARRAY_JOBID(job->jobId), endTime, nowTime, lingerTime);
			}

			if (endTime > 0) {
				if (nowTime - endTime <= lingerTime) {
					if (debug) {
						printf("elim.reaper: LOADING: Finished Job in Linger Range.\n");
					}

					snprintf(users[i].user, 40, "%s", job->user);
					i++;
				} else if (debug) {
					printf("elim.reaper: SKIPPING: Finished Job outside linger window.\n");
				}
			} else {
				if (debug) {
					printf("elim.reaper: LOADING: Active Job.\n");
				}

				snprintf(users[i].user, 40, "%s", job->user);
				i++;
			}

			if (i > maxJobUsers) {
				syslog(LOG_WARNING, "elim.reaper: host job number exceeds %s!", maxJobUsers);
				return i;
			}

			if (!more) {
				return i;
			}
		}
	}
}

int is_lsf_admin(const char *user) {
	int i;
	struct clusterInfo *cluster;
	static admins_t *lsfAdmins;
	static int numAdmins = 0;

	if (debug) {
		printf("elim.reaper: Checking if user (%s) is and LSF Admin\n", user);
	}

	if (lsfAdmins == NULL) {
		cluster = ls_clusterinfo(NULL, NULL, NULL, 0, 0);

		if (cluster != NULL) {
			lsfAdmins = (admins_t *) calloc(cluster->nAdmins, sizeof(admins_t));
			numAdmins = cluster->nAdmins;
			for (i = 0; i < cluster->nAdmins; i++) {
				snprintf(lsfAdmins[i].admin, 40, "%s", cluster->admins[i]);
			}
		} else {
			syslog(LOG_NOTICE, "elim.reaper: LSF unreachable.  Allowing access for user (%s)", user);
			return TRUE;
		}
	}

	if (lsfAdmins == NULL) {
		syslog(LOG_NOTICE, "elim.reaper: LSF not returning admins.  Allowing access for user (%s)", user);
		return TRUE;
	} else {
		for (i = 0; i < numAdmins; i++) {
			if (debug) {
				syslog(LOG_NOTICE, "elim.reaper: comparing cluster admin %s", lsfAdmins[i].admin);
			}

			if (strcmp(user, lsfAdmins[i].admin) == 0) {
				if (debug) {
					syslog(LOG_NOTICE, "elim.reaper: the following users is an LSF Admin (%s)", lsfAdmins[i].admin);
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

/*! \fn static void display_help()
 *  \brief Display Spine usage information to the caller.
 *
 *  Display the help listing: the first line is created at runtime with
 *  the version information, and the rest is strictly static text which
 *  is dumped literally.
 *
 */
static void display_help(int only_version) {
	static const char *const *p;
	static const char * const helptext[] = {
		"Usage: elim.reaper [options]",
		"",
		"Options:",
		"  -s/--sleep=X       Sleep X seconds before checking again",
		"  -h/-H/--help       Show this brief help listing",
		"  -v/-V/--version    Show the tool version",
		"  -d/--debug         Provide verbose debug information",
		"",
		"This program supports a configuration file called lsf.reaper.  The lsf.reaper",
		"file must be placed in the $LSF_ENVDIR.",
		"",
		"If the file is not present, then if LSF settings do not permit user processes",
		"to linger after job termination, then this elim will immediately kill all",
		"left over user login processes as soon as that login users jobs are complete.",
		""
		"Otherwise, LSF will wait the LSF_LINGER_TIME before killing user sessions for",
		"non job users.",
		"",
		"LSF Administrators will not have their login sessions removed from the system.",
		"",
		"The values supported in the lsf.reaper file include:",
		"",
		"LSF_LINGER_TIME = X      Number of minutes a user process can run after all jobs",
		"                         for that user have been gone from the LSF server.",
		"LSF_SLEEP_TIME = X       Number of seconds before rechecking for processes that",
		"                         need to leave the system.",
		"",
		"LSF_EXCLUDED_HOSTS = X   Space delimited list of hosts to exclude from reaper",
		"                         checking.  These would include login nodes where",
		"                         interactive logins are permitted by non LSF Admins.",
		"",
		"Additionally, to leverage this tool, NEWJOB_REFRESH must be set to 'Y' in",
		"in lsb.params and it is recommended that you set an LSF_LINGER_TIME",
		"large enough for LSF Post processing to be done if JOB_INCLUDE_POSTPROC",
		"is set to 'N' in your lsb.params.",
		"",
		"This tool is distributed under the Terms of the Eclipse Public License",
		"Version 1.0.",

        0 /* ENDMARKER */
    };

    printf("elim.reaper %s Copyright 2018-2019 Internaltional Business Machines, Inc.\n", VERSION);

    if (only_version == FALSE) {
        printf("\n");
        for (p = helptext; *p; p++) {
            puts(*p);
        }
    }
}

