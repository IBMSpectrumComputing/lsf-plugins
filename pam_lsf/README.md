# Pluggable Authentication Module for Spectrum LSF

## How to Build and Install the Module

1. (Optional) Edit Makefile and update compiler or linker options

2. Source your LSF environment using either (profile.lsf or cshrc.lsf)

3. Run `make clean;make`

4. Copy pam_lsf.so to `/lib64/security/` or `/lib/security` depending on your OS flavor.

5. Suggest adding the following to `/etc/pam.d/sshd`

	`account required /lib64/security/pam_lsf.so [debug] LSF_ENVDIR=/usr/share/lsf/conf LSF_SERVERDIR=/usr/share/lsf/10.1/etc`

	The "debug", if used, should be the first option specified. All logging goes to syslog.  Change the paths in the LSF_ENVDIR and LSF_SERVERDIR to your production values.

6. This PAM module does allow access for known LSF administrators but if you wish to allow access for other special groups, for example the user group whistle, add the following in front of the pam_lsf.so

	`account sufficient /lib64/security/pam_access.so`

	Then create the file `/etc/security/access.conf` using the following syntax:

	`+:whistle:ALL`
	`-:ALL:ALL`

## General Usage

This pam module currently includes both "auth" and "account" functions.  They will both return PAM_SUCCESS if the user is an LSF Administrator, or the user has a job on the current host.  In the case where LSF is down, logins will be allowed.

It's important that you remake this module when you upgrade LSF, though under most cases, the module should continue to work as is.

Ensure that you pass both the LSF_SERVERDIR and LSF_ENVDIR to the pam module else you will likely encounter problems with the module.

