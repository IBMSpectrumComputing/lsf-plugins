/* pam_lsf module */

/*
 * Originally written by Mahmoud Hanafi <hanafim@asc.hpc.mil>
 * 
 * Enhanced by Larry Adams <adamsla@us.ibm.com>
 *
 */

#define DEFAULT_USER "nobody"
#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <strings.h>
#include <sys/utsname.h>
#include <lsf/lsbatch.h>
#include <lsf/lsf.h>

/*
 * here, we make definitions for the externally accessible functions
 * in this file (these definitions are required for static modules
 * but strongly encouraged generally) they are used to instruct the
 * modules include file to define their prototypes.
 */

#define PAM_SM_AUTH
#define PAM_SM_ACCOUNT
#define PAM_SM_PASSWORD
#define PAM_SM_SESSION

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/_pam_macros.h>

/* function to check for running LSF jobs */
int lsf_check(const char *username, char *hostname, int *debug) {
	int options = CUR_JOB;
	int jobs;

	/* initializing LSF */
	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: initializing LSF");
	}

	if (lsb_init("pam_lsf") < 0) {
		syslog(LOG_NOTICE, "pam_lsf.so: lsb_init() failed allowing login");
		return TRUE;
	}

	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: checking LSF user=%s hostname=%s", username, hostname);
	}

	jobs = lsb_openjobinfo(0, NULL, (char *) username, NULL, hostname, options);

	lsb_closejobinfo();

	if (jobs < 1) {
		return FALSE;
	} else {
		return TRUE;
	}
}

int lsf_admin_check(const char *user, int *debug) {
	int i;
	struct clusterInfo *cluster;

	cluster = ls_clusterinfo(NULL, NULL, NULL, 0, 0);

	if (cluster != NULL) {
		for (i = 0; i < cluster->nAdmins; i++) {
			if (debug) {
				syslog(LOG_NOTICE, "pam_lsf.so: comparing cluster admin %s", cluster->admins[i]);
			}

			if (strcmp(user, cluster->admins[i]) == 0) {
				if (debug) {
					syslog(LOG_NOTICE, "pam_lsf.so: allowing access for admin (%s)", cluster->admins[i]);
				}

				return TRUE;
			}
		}

		return FALSE;
	} else {
		syslog(LOG_NOTICE, "pam_lsf.so: LSF unreachable.  Allowing access for user (%s)", user);
		return TRUE;
	}
}

int parse_args(int argc, const char **argv, int *debug) {
	int i;

	*debug = FALSE;

	if (argc < 1) {
		return 1;
	}

	for (i = 0; i < argc; i++) {
		if (strncasecmp(argv[i], "LSF_SERVERDIR", 16)) {
			syslog(LOG_NOTICE, "pam_lsf.so: argv[%i] = %s", i, argv[i]);
			putenv((char *) argv[i]);
		} else if (strncasecmp(argv[i], "LSF_ENVDIR", 10)) {
			syslog(LOG_NOTICE, "pam_lsf.so: argv[%i] = %s", i, argv[i]);
			putenv((char *) argv[i]);
		} else if (strncasecmp(argv[i], "debug", 5)) {
			*debug = TRUE;
			syslog(LOG_NOTICE, "pam_lsf.so: argv[%i] = %s", i, argv[i]);
		}
	}	

	return 0;
}

/* supported pam functions */

PAM_EXTERN
int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	int retval;
	const char *user = NULL;
	struct utsname name;
	int debug = 0;

	if (parse_args(argc, argv, &debug) == 1) {
		syslog(LOG_ERR, "pam_lsf.so: argument parsing failed");
	}

	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: parsing done");
	}

	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: starting auth");
	}
    
	retval = pam_get_user(pamh, &user, NULL);

	if (retval != PAM_SUCCESS) {
		syslog(LOG_ERR, "pam_lsf.so: get user returned error: %s", pam_strerror(pamh,retval));
		return retval;
	}

	if (user == NULL || *user == '\0') {
		syslog(LOG_ERR, "pam_lsf.so: username not known");

		retval = pam_set_item(pamh, PAM_USER, (const void *) DEFAULT_USER);

		if (retval != PAM_SUCCESS) {
			return PAM_USER_UNKNOWN;
		}
	}

	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: username = %s", user);
	}

	/* get hostname where we are running */
	if (uname(&name) == -1) {
		syslog(LOG_ERR, "pam_lsf.so: couldn't get hostname");
		return PAM_AUTH_ERR;
	}

	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: hostname = %s", name.nodename);
	}

	/* allow root logins */
	if (strncasecmp(user, "root", 4)) {
		return PAM_SUCCESS;
	}

	if (lsf_admin_check(user, &debug)) {
		syslog(LOG_NOTICE, "pam_lsf.so: auth success for LSF admin (%s)", user);
		return PAM_SUCCESS;
	}

	if (!lsf_check(user, name.nodename, &debug)) {
		syslog(LOG_NOTICE, "pam_lsf.so: auth failed no jobs for (%s)", user);
		return PAM_AUTH_ERR;
	} else {
		syslog(LOG_NOTICE, "pam_lsf.so: auth success for (%s)", user);
		return PAM_SUCCESS;
	}
}

PAM_EXTERN
int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	syslog(LOG_NOTICE, "pam_lsf.so: returing success from setcred");
	return PAM_SUCCESS;
}

/* --- account management functions --- */

PAM_EXTERN
int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	int retval;
	const char *user = NULL;
	struct utsname name;
	int debug = 0;

	if (parse_args(argc, argv, &debug) == 1) {
		syslog(LOG_ERR, "pam_lsf.so: argument parsing failed");
	}

	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: parsing done");
	}
	
	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: starting acct_mgmt");
	}
    
	retval = pam_get_user(pamh, &user, NULL);

	if (retval != PAM_SUCCESS) {
		syslog(LOG_ERR, "pam_lsf.so: get user returned error: %s", pam_strerror(pamh,retval));
		return retval;
	}

	if (user == NULL || *user == '\0') {
		syslog(LOG_ERR, "pam_lsf.so: username not known");

		retval = pam_set_item(pamh, PAM_USER, (const void *) DEFAULT_USER);

		if (retval != PAM_SUCCESS) {
			return PAM_USER_UNKNOWN;
		}
	}

	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: username = %s", user);
	}

	/* get hostname where we are running */
	if (uname(&name) == -1) {
		syslog(LOG_ERR, "pam_lsf.so: couldn't get hostname");
		return PAM_AUTH_ERR;
	}

	if (debug) {
		syslog(LOG_NOTICE, "pam_lsf.so: hostname = %s", name.nodename);
	}

	/* allow root logins */
	if (strncasecmp(user, "root", 4)) {
		return PAM_SUCCESS;
	}

	if (lsf_admin_check(user, &debug)) {
		syslog(LOG_NOTICE, "pam_lsf.so: acct_mgmt success for LSF admin (%s)", user);

		return PAM_SUCCESS;
	}

	if (!lsf_check(user, name.nodename, &debug)) {
		syslog(LOG_NOTICE, "pam_lsf.so: acct_mgmt failed no jobs for (%s)", user);
		return PAM_AUTH_ERR;
	} else {
		syslog(LOG_NOTICE, "pam_lsf.so: acct_mgmt success for (%s)", user);
		return PAM_SUCCESS;
	}
}

PAM_EXTERN
int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return PAM_IGNORE;
}

PAM_EXTERN
int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return PAM_IGNORE;
}

PAM_EXTERN
int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return(PAM_IGNORE);
}

/* end of module definition */

#ifdef PAM_STATIC

/* static module data */

struct pam_module _pam_permit_modstruct = {
	"pam_permit",
	pam_sm_authenticate,
	pam_sm_setcred,
	pam_sm_acct_mgmt,
	pam_sm_open_session,
	pam_sm_close_session,
	pam_sm_chauthtok
};

#endif

