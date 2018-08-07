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

## Community Contribution Requirements

Community contributions to this repository must follow the [IBM Developer's Certificate of Origin (DCO)](https://github.com/IBMSpectrumComputing/platform-python-lsf-api/blob/master/IBMDCO.md) process and only through GitHub Pull Requests:

 1. Contributor proposes new code to community.

 2. Contributor signs off on contributions 
    (i.e. attachs the DCO to ensure contributor is either the code 
    originator or has rights to publish. The template of the DCO is included in
    this package).
 
 3. IBM Spectrum LSF development reviews contribution to check for:
    i)  Applicability and relevancy of functional content 
    ii) Any obvious issues

 4. If accepted, posts contribution. If rejected, work goes back to contributor and is not merged.

## Copyright

(C) Copyright IBM Corporation 2016-2018

U.S. Government Users Restricted Rights - Use, duplication or disclosure 
restricted by GSA ADP Schedule Contract with IBM Corp.

IBM(R), the IBM logo and ibm.com(R) are trademarks of International Business Machines Corp., 
registered in many jurisdictions worldwide. Other product and service names might be trademarks 
of IBM or other companies. A current list of IBM trademarks is available on the Web at 
"Copyright and trademark information" at www.ibm.com/legal/copytrade.shtml.
