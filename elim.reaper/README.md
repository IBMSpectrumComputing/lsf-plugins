# Non Job User Process Reaper Service for IBM Spectrum LSF

## Description

This Non Job User Process Reaper for LSF will remove any unauthroized users from the
compute hosts once their jobs have left those system.  It provides an optional grace period called the LSF_LINGER_TIME that you can set in the lsf.reaper file in your $LSF_ENVDIR.  The value is in seconds.

You can optionally set a LSF_SLEEP_TIME to a value in seconds as well.  As you clusters become larger, you may not want them querying the batch system as much, so increasing this value may be practical in such cases.  This value is also in seconds.

## How to Build and Install the Module

1. (Optional) Edit Makefile and update compiler or linker options
2. Source your LSF environment using either (profile.lsf or cshrc.lsf)
3. Run `make clean;make`
4. Run `make install` or copy elim.reaper to ${LSF_SERVERDIR}, and the default lsf.reaper to the ${LSF_ENVDIR}

## Installation Instructions

To install this elim, you must first add the "reaper" numeric resource to LSF as per the normal process which involves updating your lsf.shared and lsf.cluster files to include the value.  Ensure that you assign this resource to hosts that you wish to clear user processes from automatically.

Then, before restarting the cluster, make sure that the elim.reaper binary has been copied to the $LSF_SERVERDIR for all compute hosts, and that the lsf.reaper file has been copied to the $LSF_ENVDIR of your cluster or hosts.  After which, you can restart your cluster using:

`lsadmin reconfig`
`badmin reconfig`

Make sure you restart all LIM's and not just the Master LIM.  From each compute host, you should then see the binary running in the background.  If not, you should debug the binary interactively using the --debug option.

## General Usage

This Non Job User Process Reaper for LSF will remove any unauthroized users from the
compute hosts once their jobs have left the system.  It provides an optional grace period called the LSF_LINGER_TIME that you can set in the lsf.reaper file in your $LSF_ENVDIR.  The value is in seconds.

You can optionally set a LSF_SLEEP_TIME to a value in seconds as well.  As you clusters become larger, you may not want them querying the batch system as much, so increasing this value may be practical in such cases.  This value is also in seconds.

If you wish to see debug output from the elim, you may run it with the --debug option.

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

