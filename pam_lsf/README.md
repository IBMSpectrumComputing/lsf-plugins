# Pluggable Authentication Module for IBM Spectrum LSF

## How to Build and Install the Module

1. (Optional) Edit Makefile and update compiler or linker options
2. Source your LSF environment using either (profile.lsf or cshrc.lsf)
3. Run `make clean;make`
4. Run `make install` or copy pam_lsf.so to `/lib64/security/`, `/lib/security`, or your operating systems pam security folder.
5. Add the following to `/etc/pam.d/sshd`:

	`account required /lib64/security/pam_lsf.so [debug] LSF_ENVDIR=path LSF_SERVERDIR=path`

	The "debug" option, if used, should be the first option specified. All logging goes to syslog.  Change the paths in the LSF_ENVDIR and LSF_SERVERDIR to your production values.  In this example, we use the Red Hat and SLES default locations for the pam_lsf.so shared library, but your OS flavor may differ.  For example, Debian based OS' like Ubuntu use a different location.

6. This PAM module does allow access for known LSF administrators but if you wish to allow access for other special groups, for example the user group whistle, add the following before of the pam_lsf.so

	`account sufficient /lib64/security/pam_access.so`

	Then create the file `/etc/security/access.conf` using the following syntax:

	+:whistle:ALL</br>
	-:ALL:ALL

## General Usage

This PAM module currently includes both "auth" and "account" functions.  They will both return PAM\_SUCCESS if the user is an LSF Administrator, or the user has a job on the current host.  In the case where LSF is down, logins will be allowed.

It's important that you plan to remake this module when you upgrade LSF, though under most cases, the module should continue to work as is.

Ensure that you pass both the correct LSF_SERVERDIR and LSF_ENVDIR to the PAM module else you will likely encounter problems.  In any case, you should see messages in syslog every time a user attempts to login.

